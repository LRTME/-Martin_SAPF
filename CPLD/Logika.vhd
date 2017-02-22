------------------------------------------------------------------------------------------
-- Company:
-- Engineer:
--
-- Create Date:		12:43:15 06/07/2012
-- Design Name:
-- Module Name:		CPLD_ADC_final - Behavioral
--
------------------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- definicije vhodov in izhodov
entity Logika is
	Port(	Supply_main_mcu			: in STD_LOGIC;
			Supply_resistor_mcu		: in STD_LOGIC;
			Filter_main_mcu			: in STD_LOGIC;
			LATCH_RESET				: in STD_LOGIC;
			MOSFET_MCU				: in STD_LOGIC;
			WD_OUT					: in STD_LOGIC;
			
			TZ_IF					: in STD_LOGIC;
			TZ_UF					: in STD_LOGIC;
			TZ_IS					: in STD_LOGIC;

			Supply_main				: out STD_LOGIC;
			Filter_main				: out STD_LOGIC;
			MOSFET					: out STD_LOGIC;
			Supply_resistor			: out STD_LOGIC;
			PWM_OE					: out STD_LOGIC;
			
			trip					: inout STD_LOGIC;
			over_voltage			: inout STD_LOGIC;
			over_current_filter		: inout STD_LOGIC;
			over_current_supply		: inout STD_LOGIC);
end Logika;

architecture Behavioral of Logika is
	signal WD_LATCH		: STD_LOGIC;
begin
	-- prireditev trip izhodov
	trip <= '0' when (	over_voltage = '0'
					or	over_current_filter = '0'
					or	over_current_supply = '0'
					or	WD_LATCH = '0')
				else '1';
	
	-- prozenje kontaktorjev in kratkosticnih MOSFET-ov
	Supply_main <= '1' when (Supply_main_mcu = '1' and trip = '1')
				else '0';
	Supply_resistor <= '1' when (Supply_resistor_mcu = '1' and trip = '1')
				else '0';
	Filter_main <= '1' when (Filter_main_mcu = '1' and trip = '1')
				else '0';			
	MOSFET <= '1' when (MOSFET_MCU = '1' and trip = '1')
				else '0';
	PWM_OE <= '0' when (trip = '1')
				else '1';
				
	-- over_voltage latch
	over_voltage <= (LATCH_RESET nand TZ_UF) nand (TZ_UF nand over_voltage);
	-- over_current_filter latch
	over_current_filter <= (LATCH_RESET nand TZ_IF) nand (TZ_IF nand over_current_filter);
	-- over_current_supply latch
	over_current_supply <= (LATCH_RESET nand TZ_IS) nand (TZ_IS nand over_current_supply);
	-- WD_LATCH latch
	WD_LATCH <= (LATCH_RESET nand WD_OUT) nand (WD_OUT nand WD_LATCH);
	
end Behavioral;