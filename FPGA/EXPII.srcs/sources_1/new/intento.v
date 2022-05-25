
architecture Behavioral of sm is
signal r_write_enable : std_logic := '0';
signal r_X : unsigned (9 downto 0) := (others => '0') ;
signal edge : std_logic := '0';
signal edge_X : std_logic := '0';
signal count : unsigned (9 downto 0) := (OTHERS => '0');


begin
process(Clk)
begin
  if rising_edge(Clk) then 
    r_write_enable<=write_enable;
    r_X <= X;
  end if;  
end process;

process(Clk, reset) 
begin  
  if reset= '1' then 
    count <= (OTHERS => '0');  
  elsif rising_edge(Clk) then
    if edge='1' then 
      if count<=640 then
      count <= count + 1; 
      addr_a <= std_logic_vector(count);
      else 
        count <= (OTHERS => '0');
      end if;
    elsif edge_X='1' then -- se está haciendo en cada clk y no en cada X
      addr_a <= std_logic_vector(X);
      X_point<= std_logic_vector(X);
      Y_point <= RAM_output;
    end if;
  end if;
end process;

edge <= write_enable and (not  r_write_enable ); --Detector de flanco
edge_X <= X(0) and (not  r_X(0)); --Detector de flanco
end Behavioral;