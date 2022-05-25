library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity uart_rx is
  generic (
    BAUDRATE : integer := 115200; -- Desired baudrate
    CLK_FREQ : integer := 50000000 -- Input clock in Hz
  );
  port (
    reset : in std_logic;
    clk : in std_logic;
    rx : in std_logic;
    data_out : out std_logic_vector (7 downto 0);
    ready : out std_logic
  );
end uart_rx;
architecture uart_rx_arch of uart_rx is

  constant OVERSAMPLING : integer := 4;
  constant RX_CLK_PRESCALE : integer := CLK_FREQ / (OVERSAMPLING * BAUDRATE);

  type uart_rx_states is (WAIT_START, SET_PRSC, RECEIVE, WAIT_STOP);
  signal state : uart_rx_states;

  signal rx_clk_prsc : std_logic_vector(14 downto 0) := (others => '0'); -- rx_clk prescaler
  signal rx_clk : std_logic := '0'; -- 4 x bit clock
  signal bit_cnt : std_logic_vector(3 downto 0) := (others => '0'); -- RX bit counter
  signal oversample_cnt : std_logic_vector(1 downto 0) := (others => '0'); -- oversampling counter
  signal shreg : std_logic_vector(7 downto 0) := (others => '0');

begin

  -- rx_clk generation
  process (reset, clk)
  begin
    if (reset = '0') then
      rx_clk_prsc <= (others => '0');
      rx_clk <= '0';
    elsif rising_edge(clk) then
      if (to_integer(UNSIGNED(rx_clk_prsc)) < (RX_CLK_PRESCALE - 1)) then
        rx_clk_prsc <= std_logic_vector(UNSIGNED(rx_clk_prsc) + 1);
        rx_clk <= '0';
      else
        rx_clk_prsc <= (others => '0');
        rx_clk <= '1';
      end if;
    end if;
  end process;

  -- receiving state machine
  process (reset, clk)
  begin
    if (reset = '0') then
      state <= WAIT_START;
      data_out <= (others => '0');
      ready <= '0';
    elsif rising_edge(clk) then
      if (rx_clk = '1') then
        case state is
          when WAIT_START =>
            if (rx = '0') then
              state <= SET_PRSC;
              ready <= '0';
              oversample_cnt <= (others => '0');
            end if;

          when SET_PRSC =>
            if (to_integer(UNSIGNED(oversample_cnt)) < ((OVERSAMPLING / 2) - 2)) then
              oversample_cnt <= std_logic_vector(UNSIGNED(oversample_cnt) + 1);
            else
              oversample_cnt <= (others => '0');
              state <= RECEIVE;
              bit_cnt <= (others => '0');
            end if;

          when RECEIVE =>
            if (UNSIGNED(oversample_cnt) = 3) then
              oversample_cnt <= (others => '0');
              shreg(7) <= rx;
              shreg(6 downto 0) <= shreg(7 downto 1);
              bit_cnt <= std_logic_vector(UNSIGNED(bit_cnt) + 1);
              if (UNSIGNED(bit_cnt) = 7) then
                state <= WAIT_STOP;
              end if;
            else
              oversample_cnt <= std_logic_vector(UNSIGNED(oversample_cnt) + 1);
            end if;

          when WAIT_STOP =>
            data_out <= shreg;
            ready <= '1';
            if (rx = '1') then
              state <= WAIT_START;
            end if;
        end case;
      end if;
    end if;
  end process;

end uart_rx_arch;