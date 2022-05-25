----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
----------------------------------------------------------------------------------

entity draw is
    Port ( clk: in std_logic;
           X : in unsigned (9 downto 0);
           Y : in unsigned (9 downto 0);
           Y_point : in unsigned (7 downto 0); 
           offset: in unsigned (7 downto 0);
           volts_div: in unsigned (7 downto 0);  
           tim_div: in unsigned (7 downto 0);
           RGB : out STD_LOGIC_VECTOR (11 downto 0));
end draw;

architecture Behavioral of draw is


type img is array( 0 to 7, 0 to 3) of std_logic;
type num is array(0 to 9) of img;
CONSTANT numeros: num := (
    (   "1111",
        "1001",
        "1001",
        "1001",
        "1001",
        "1001",
        "1001",
        "1111"
        ),
    ( "0010", 
    "0110", 
    "0010",
    "0010",
    "0010",
    "0010",
    "0010",
    "0111"),

    ("1111",
    "0001",
    "0001",
    "1111", 
    "1000", 
    "1000", 
    "1000",
    "1111"),

    ("1111",
    "0001",
    "0001",
    "1111",
    "0001",
    "0001",
    "0001",
    "1111"),

    ("1001",
    "1001",
    "1001",
    "1001",
    "1111",
    "0001",
    "0001",
    "0001"),

    ("1111",
    "1000",
    "1000",
    "1111",
    "0001",
    "0001",
    "0001",
    "1111"),

    ("1111",
    "1000",
    "1000",
    "1111",
    "1001",
    "1001",
    "1001",
    "1111"),

    ("1111",
    "0001",
    "0010",
    "0010",
    "0100",
    "0100",
    "1000",
    "1000"),

    ("1111",
    "1001",
    "1001",
    "1111",
    "1001",
    "1001",
    "1001",
    "1111"),
    
    ("1111",
    "1001",
    "1001",
    "1111",
    "0001",
    "0001",
    "0001",
    "1111")
);
Constant N1 : integer := 17;
Constant N2 : integer := 3;
Constant base_time : integer := 5;
type eje_X is array (0 to N1) of integer range 0 to 640;
--type ejeXvalue is array (0 to N1) of integer range 0 to 9;
type ejeXvalue is array (0 to 1) of integer range 0 to 9;
type eje_Y is array (0 to N2) of integer range 0 to 640;
type ejeYvalue is array (0 to N2) of integer range 0 to 9;
Constant ejeX: eje_X := (
    54,59,118,123,182,187,246,252,310,315,374,379,438,443,502,507,566,571);
Constant ejeY: eje_Y := (
    173,235,299,364);

signal Y_point_cast: unsigned (9 downto 0) := (others=>'0');
signal Y_prev_cast: unsigned (9 downto 0) := (others=>'0');

signal r_X : std_logic := '0';
signal r_X2 : std_logic;
signal X_edge : std_logic;
signal counter_edgeX: integer range 0 to N1 := 0;
signal counter_edgeY: integer range 0 to N2 := 0;
signal counter_num: integer range 0 to 1 := 0;
Constant ejeY1: integer := 380;
Constant ejeX1: integer := 325;
signal ejeYval: integer range 0 to 5 := 1;
signal ejeXval: ejeXvalue := (0,0);
--signal ejeXval: ejeXvalue := (
--   0,5,1,0,1,5,2,0,2,5,3,0,3,5,4,0,4,5);
   
begin
    Y_point_cast <= unsigned("00" & std_logic_vector(Y_point))+ offset;

    process(Y,X,Y_point_cast,Y_prev_cast, X_edge)
    begin  
            if Y=Y_point_cast then 
                RGB<="111100000000"; 

            elsif ((Y<Y_point_cast and Y>=Y_prev_cast) or (Y>Y_point_cast and Y<=Y_prev_cast)) and X>1 then
                RGB<="111100000000";   

            elsif (Y>=ejeY1 and Y<ejeY1+8) and (X>=ejeX(counter_edgeX) and X<(ejeX(counter_edgeX)+4)) then               
                if numeros(ejeXval(counter_num))(to_integer(Y)-ejeY1, to_integer(X)-ejeX(counter_edgeX)) = '1' then                                
                    RGB<=(others => '0');  
                else
                    RGB<=(others => '1');
                end if;
            elsif (Y>=ejeY(counter_edgeY) and Y<ejeY(counter_edgeY)+8) and (X>=ejeX1 and X<(ejeX1+4)) then
                if numeros(ejeYval)(to_integer(Y)-ejeY(counter_edgeY), to_integer(X)-ejeX1) = '1' then                              
                    RGB<=(others => '0');
                                    
                else
                    RGB<=(others => '1');
                end if;                                                  
                       
            elsif X>0 and X<5 and Y<offset+131 and Y>offset+125 then 
                RGB<="111100000000"; 
            elsif Y=120 or Y=375 or Y=247 or (X=320 and Y > 120 and Y < 375)then
                RGB<="100010011001";   
            elsif X=64 or X=128 or X=192 or X=256 or X=320 or X=384 or X=448 or X=512 or X=576 then 
                RGB<="110111011101"; 
            elsif Y=56 or Y=120 or Y=186 or Y=248 or Y=312 or Y=376 or Y=440 then 
                RGB<="110111011101";         
            else
                RGB<=(others => '1'); 
            end if;                             
     end process;

  
  process (clk,X,Y,X_edge,counter_edgeX)
  begin --poner señal reset    
   if clk'event and clk='1' then 
    r_X <= X(0); 
    if X_edge = '1' then                   
        Y_prev_cast <= Y_point_cast;                        
    else 
        Y_prev_cast <= Y_prev_cast;
    end if;   
    --if X_edge= '1' and X=ejeX(counter_edgeX)+4 and counter_edgeX < N-1 and (ejeX(counter_edgeX)=ejeX(counter_edgeX+1) or Y=ejeY(counter_edgeX)+7) then
    --if X_edge= '1' and X=ejeX(counter_edgeX)+4 and counter_edgeX < N1  then
    if X_edge= '1' and X=ejeX(counter_edgeX)+4 then
        if counter_edgeX < N1  then
            counter_edgeX <= counter_edgeX + 1;
        else
            counter_edgeX <= 0;  
        end if; 
        if counter_num <1  then
            counter_num <= 1;
        else
            counter_num <= 0;  
        end if;   
    end if;     
    if X_edge= '1' and Y=ejeY(counter_edgeY)+8 and X=ejeX1+4 and counter_edgeY < N2  then
        counter_edgeY <= counter_edgeY + 1;
    elsif X_edge= '1' and Y=ejeY(counter_edgeY)+8 and X=ejeX1+4 then
        counter_edgeY <= 0;    
    end if;    
   end if;
   end process;  

X_edge <= (r_X and not X(0)) or (not r_X and X(0));
ejeYval<=to_integer(volts_div);
ejeXval(0)<=to_integer(tim_div)*base_time/10;
process (tim_div)
begin
    if tim_div=1 then 
        ejeXval(1)<= 5;
    else         
        ejeXval(1)<= 0;
    end if;
end process;

end Behavioral;
