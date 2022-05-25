----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;

----------------------------------------------------------------------------------

entity sistema is
  port (
    rx_ch1 : in std_logic;
    rx_ch2 : in std_logic;
    Clk : in std_logic;
    Reset : in std_logic;
    Hsync : out std_logic;
    Vsync : out std_logic;
    vgaRed, vgaGreen, vgaBlue : out std_logic_vector(3 downto 0);
    led1 : out std_logic);
end sistema;

architecture Behavioral of sistema is

  signal RGB : std_logic_vector(11 downto 0); --red:4bits, green:4bits, blue:4bits
  signal X : unsigned (9 downto 0);
  signal Y : unsigned (9 downto 0);

  signal volts_div : unsigned (7 downto 0);
  signal tim_div : unsigned (7 downto 0);

  signal Y_point_ch1 : std_logic_vector (7 downto 0);
  signal dina_ch1 : std_logic_vector(7 downto 0) := "00000000";
  signal addr_a_ch1 : std_logic_vector(9 downto 0);
  
  signal RAM_output_ch1 : std_logic_vector (7 downto 0);
  signal offset_ch1 : unsigned (7 downto 0);
  signal ch_enable_ch1: std_logic;
  signal ready_ch1: std_logic;
  signal  wea_ch1 : std_logic_vector (0 downto 0);

  signal Y_point_ch2 : std_logic_vector (7 downto 0);
  signal dina_ch2 : std_logic_vector(7 downto 0) := "00000000";
  signal addr_a_ch2 : std_logic_vector(9 downto 0);
  
  signal RAM_output_ch2 : std_logic_vector (7 downto 0);
  signal offset_ch2 : unsigned (7 downto 0);
  signal ch_enable_ch2: std_logic;
  signal ready_ch2: std_logic;
  signal  wea_ch2 : std_logic_vector (0 downto 0);
  
  constant baudrate : integer := 100000;
  constant freq_rx : integer := 100000000;
 

  component vga
    port (
      Clk : in std_logic;
      Reset : in std_logic;
      Color : in std_logic_vector (11 downto 0);
      HSync : out std_logic;
      VSync : out std_logic;
      R, G, B : out std_logic_vector(3 downto 0);
      X : out unsigned (9 downto 0);
      Y : out unsigned (9 downto 0));
  end component;

  component draw
    port (
    clk : in std_logic;
    X : in unsigned (9 downto 0);
    Y : in unsigned (9 downto 0);
    Y_point : in unsigned (7 downto 0);
    offset: in unsigned (7 downto 0);
    volts_div: in unsigned (7 downto 0);
    tim_div: in unsigned (7 downto 0);
    RGB : out STD_LOGIC_VECTOR (11 downto 0)

  );
  end component;


  component blk_mem_gen_0
    port (
    clka : in STD_LOGIC;
    ena : in STD_LOGIC;
    wea : in STD_LOGIC_VECTOR ( 0 to 0 );
    addra : in STD_LOGIC_VECTOR ( 9 downto 0 );
    dina : in STD_LOGIC_VECTOR ( 7 downto 0 );
    douta : out STD_LOGIC_VECTOR ( 7 downto 0 )
  );
  end component;

  component sm is
    Port ( 
          clk: in std_logic;       
          write_enable : in std_logic;
          X : in unsigned (9 downto 0);
          reset : in std_logic;
          RAM_output : in std_logic_vector  (7 downto 0);
          RAM_input : in std_logic_vector  (7 downto 0);        
          addr_a :  out std_logic_vector  (9 downto 0);  
          offset: out unsigned (7 downto 0);  
          ch_enable: out std_logic;
          volts_div: out unsigned (7 downto 0); 
          tim_div: out unsigned (7 downto 0);     
          wea : out std_logic_vector (0 downto 0)
          );
  end component;

component uart
  generic (
    BAUDRATE : INTEGER;
    CLK_FREQ : INTEGER
  );
    port (
    reset : in STD_LOGIC;
    clk : in STD_LOGIC;
    data_in : in STD_LOGIC_VECTOR ( 7 DOWNTO 0 );
    load : in STD_LOGIC;
    rx : in STD_LOGIC;
    data_out : out STD_LOGIC_VECTOR ( 7 DOWNTO 0 );
    ready : out STD_LOGIC;
    sent : out STD_LOGIC;
    tx : out STD_LOGIC
  );
end component;

begin
  driver : vga
  port map(
    Clk => Clk,
    Reset => Reset,
    Color=> RGB,
    HSync => Hsync,
    VSync => Vsync,
    R => vgaRed,
    G => vgaGreen,
    B => vgaBlue,
    X => X,
    Y => Y);

  circuit : draw
  port map(
    clk => clk,
    X => X,
    Y => Y,
    Y_point => unsigned(RAM_output_ch1), 
    offset => offset_ch1,
    volts_div => volts_div,
    tim_div => tim_div,
    RGB => RGB);

  bram : blk_mem_gen_0
  port map (
    clka => Clk,
    ena => '1',
    wea => wea_ch1,
    addra => addr_a_ch1,
    dina => dina_ch1,
    douta => RAM_output_ch1
  ); 

  state : sm 
  port map (
    clk => clk,
    write_enable => ready_ch1,
    X => X,
    reset => Reset,
    RAM_output => RAM_output_ch1,
    RAM_input => dina_ch1,
    addr_a => addr_a_ch1,
    offset => offset_ch1,
    ch_enable => ch_enable_ch1,
    volts_div => volts_div,
    tim_div => tim_div,
    wea => wea_ch1
  );

  uart_inst_ch1 : uart
    GENERIC map (
    BAUDRATE => baudrate,
    CLK_FREQ => freq_rx
  )
    port map (
    reset => not Reset,
    clk => Clk,
    data_in => (others => '0') ,
    load => '0',
    rx => rx_ch1,
    data_out => dina_ch1,
    ready => ready_ch1,
    sent => open,
    tx => open
  );
led1 <= rx_ch1;
end Behavioral;