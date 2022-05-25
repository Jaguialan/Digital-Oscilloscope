LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;

ENTITY uart IS
  GENERIC (
    BAUDRATE : INTEGER := 115200;
    CLK_FREQ : INTEGER := 100000000
  );
  PORT (
    reset    : IN  STD_LOGIC;
    clk      : IN  STD_LOGIC;
    data_in  : IN  STD_LOGIC_VECTOR ( 7 DOWNTO 0 );
    load     : IN  STD_LOGIC;
    rx       : IN  STD_LOGIC;
    data_out : OUT STD_LOGIC_VECTOR ( 7 DOWNTO 0 );
    ready    : OUT STD_LOGIC;
	  sent     : OUT STD_LOGIC;
    tx       : OUT STD_LOGIC
  );
END uart;

ARCHITECTURE uart_arch OF uart IS  

  COMPONENT uart_rx IS
    GENERIC (
      BAUDRATE : INTEGER := 115200;     -- Desired baudrate
      CLK_FREQ : INTEGER := 100000000  -- Input clock in Hz
    );
    PORT (
      reset    : IN  STD_LOGIC;
      clk      : IN  STD_LOGIC;
      rx       : IN  STD_LOGIC;
      data_out : OUT STD_LOGIC_VECTOR ( 7 DOWNTO 0 );
      ready    : OUT STD_LOGIC
    );
  END COMPONENT;
  
BEGIN

    rx_i : uart_rx
    GENERIC MAP (
      BAUDRATE => BAUDRATE, -- Desired baudrate
      CLK_FREQ => CLK_FREQ  -- Input clock in Hz
    )
    PORT MAP (
      reset    => reset,
      clk      => clk,
      data_out => data_out,
      ready    => ready,
      rx       => rx
    );

END uart_arch;
