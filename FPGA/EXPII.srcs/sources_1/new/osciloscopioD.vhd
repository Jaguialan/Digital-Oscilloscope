----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 06.04.2022 12:29:11
-- Design Name: 
-- Module Name: osciloscopioD - Behavioral
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


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity osciloscopioD is
--  Port ( );
end osciloscopioD;

architecture Behavioral of osciloscopioD is
    component sistema
        port (
        Clk : in std_logic;
        Reset : in std_logic;
        Hsync : out std_logic;
        Vsync : out std_logic;
        vgaRed : out std_logic_vector(3 downto 0);
        vgaGreen : out std_logic_vector(3 downto 0);
        vgaBlue : out std_logic_vector(3 downto 0)
      );
    end component;
    
PROCESS(X, Y)
begin
IF Y=380 THEN
    COLOR <= "000";
ELSE IF X=20  THEN
    COLOR <= "000";
ELSE
    COLOR <= "111";
END IF;
END PROCESS;

end osciloscopioD;
