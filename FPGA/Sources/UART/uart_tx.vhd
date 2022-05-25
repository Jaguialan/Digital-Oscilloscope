LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;

ENTITY uart_tx IS
  GENERIC (
    BAUDRATE : INTEGER := 115200;     -- Desired baudrate
    CLK_FREQ : INTEGER := 100000000  -- Input clock in Hz
  );
  PORT (
    reset   : IN  STD_LOGIC;
    clk     : IN  STD_LOGIC;
    data_in : IN  STD_LOGIC_VECTOR ( 7 DOWNTO 0 );
    start   : IN  STD_LOGIC;
	ready	: OUT STD_LOGIC;
    tx      : OUT STD_LOGIC
  );
END uart_tx;


ARCHITECTURE uart_tx_arch OF uart_tx IS

  CONSTANT TX_CLK_PRESCALE : INTEGER := CLK_FREQ / BAUDRATE;

  TYPE uart_tx_states IS ( IDLE, SYNCH, TX_START, TRANSMIT, STOP );
  SIGNAL state : uart_tx_states;

  SIGNAL tx_clk      : STD_LOGIC := '0';
  SIGNAL tx_prsc_cnt : STD_LOGIC_VECTOR ( 14 DOWNTO 0 ) := ( OTHERS => '0' );
  SIGNAL bit_cnt     : STD_LOGIC_VECTOR ( 3 DOWNTO 0 ) := ( OTHERS => '0' );
  SIGNAL shreg       : STD_LOGIC_VECTOR ( 7 DOWNTO 0 ) := ( OTHERS => '0' );
  
BEGIN

-- TX clock generator
  PROCESS(reset, clk)
  BEGIN
    IF ( reset = '1' ) THEN
      tx_clk      <= '0';
      tx_prsc_cnt <= (OTHERS => '0');
    ELSIF rising_edge(clk) THEN
      IF( to_integer( UNSIGNED( tx_prsc_cnt ) ) < ( TX_CLK_PRESCALE - 1 ) ) THEN
        tx_prsc_cnt <= STD_LOGIC_VECTOR( UNSIGNED( tx_prsc_cnt ) + 1 );
        tx_clk      <= '0';
      ELSE
        tx_prsc_cnt <= (OTHERS => '0');
        tx_clk      <= '1';
      END IF;
    END IF;
  END PROCESS;

-- TX state machine
  PROCESS(reset, clk)
  BEGIN
    IF ( reset = '1' ) THEN
      state <= IDLE;
    ELSIF rising_edge(clk) THEN
      CASE state IS
        WHEN IDLE =>
          IF ( start = '1' ) THEN
            state <= SYNCH;
            shreg <= data_in;
          END IF;

        WHEN SYNCH =>
          IF ( tx_clk = '1' ) THEN
            state <= TX_START;
          END IF;

        WHEN TX_START =>
          bit_cnt <= (OTHERS => '0');
          IF ( tx_clk = '1' ) THEN
            state   <= TRANSMIT;
          END IF;

        WHEN TRANSMIT =>
          IF ( tx_clk = '1' ) THEN
            shreg(6 DOWNTO 0) <= shreg(7 DOWNTO 1);

            IF ( UNSIGNED( bit_cnt ) < 7 ) THEN
              bit_cnt <= STD_LOGIC_VECTOR( UNSIGNED( bit_cnt ) + 1 );
            ELSE
              state <= STOP;
            END IF;
          END IF;

        WHEN STOP =>
          IF ( tx_clk = '1' ) THEN
            state <= IDLE;
          END IF;

      END CASE;
    END IF;
  END PROCESS;

-- TX channel
  ready <= '1'   WHEN ( state = STOP ) ELSE '0';
  
  tx <= '1'      WHEN ( state = IDLE OR state = STOP OR state = SYNCH) ELSE
        '0'      WHEN state = TX_START ELSE
        shreg(0) WHEN state = TRANSMIT;

END uart_tx_arch;
