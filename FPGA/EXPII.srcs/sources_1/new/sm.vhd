----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 19.04.2022 11:59:38
-- Design Name: 
-- Module Name: sm - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.all;
--use IEEE.NUMERIC_STD.all;
use IEEE.std_logic_arith.ALL;

----------------------------------------------------------------------------------

entity sm is
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
end sm;

architecture Behavioral of sm is

type state is (read, write);
signal actual_s, next_s : state ;
signal r_write_enable : std_logic := '0';
signal edge : std_logic := '0';
signal count : unsigned (9 downto 0) := (OTHERS => '0');
signal no_save: std_logic := '0';


begin
process(reset, clk)
begin
  if reset= '1' then 
    actual_s <= read;
  elsif rising_edge(clk) then 
    actual_s <= next_s;
  end if;
end process;

process(Clk)
begin
  if rising_edge(clk) then 
    r_write_enable<=write_enable;
  end if;  
end process;

process(actual_s)
begin 

  case actual_s is

   when write =>
     if no_save = '1' then 
        wea<="0";
     else
        wea<="1";   
     end if;   
     addr_a <= std_logic_vector(count);

    if edge='1' then 
      next_s <= write;
    else 
      next_s <= read;
    end if;

   when read =>
     wea<="0"; 
     addr_a <= std_logic_vector(X);     
     if edge='1' then 
       next_s <= write;
     else 
       next_s <= read; 
     end if;
  end case;
  
end process;

process (clk,reset)
begin 
  
  if reset= '1' then
    count<=(others => '0') ;
  elsif clk'event and clk='1' then        
        if edge='1' then
          if  RAM_input = "11111111" then 
            count <= (OTHERS => '0');
          elsif count<644 then      
            count <= count + 1;
          end if;
        end if;
  end if;

end process;

process (clk,reset)
begin   
  if reset= '1' then
    offset <= "01111000";
  elsif clk'event and clk='1' then
    if count=640 then 
      offset <=unsigned(RAM_input);
    end if;
    if count=641 then 
      volts_div <=unsigned(RAM_input);
    end if;
    if count=642 then 
      tim_div <=unsigned(RAM_input);
    end if;
    if count=643 then 
      if unsigned(RAM_input)=1 then 
        ch_enable<='1';
      else 
        ch_enable<='0';
      end if;
    end if;
  end if;
end process;

process (count,RAM_input)
begin 
  if count>639 or RAM_input="11111111" then 
    no_save<='1';
  else 
    no_save<='0';
  end if;
end process;


edge <= write_enable and (not  r_write_enable ); --Detector de flanco

end Behavioral;
