local function menuable_tap()
	-- Declare the window we will use
	local tw = TextWindow.new("HID++ report data")

	-- this is our tap
	local tap = Listener.new();

	local function remove()
		-- this way we remove the listener that otherwise will remain running indefinitely
		tap:remove();
	end
	
	local function write_header()
		tw:append("Header        Body\n")
		tw:append("00 01 02 03   00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15\n\n")
	end
	
	local function write_data(report)
		local header_length = 4
		local header = report:subset(0, header_length)
		local header_hex = header:tohex(true, " ")
		
		local body_offset = header_length
		local body = report:subset(body_offset, report:len() - body_offset)
		local body_hex = body:tohex(true, " ")
		
		tw:append(header_hex .. "   " .. body_hex .. "\n")
	end

	-- we tell the window to call the remove() function when closed
	tw:set_atclose(remove)

	-- this function will be called once for each packet
	function tap.packet(pinfo,tvb)
		if (tostring(pinfo.dst) == "4.1.0") then
			local data = tvb:bytes()
			local length = data:len()
			local hex = data:tohex(true, " ")
			local longpacket = false;
			
			if(length >= 20) then
				local offset = length - 20
				local value = data:get_index(offset)
				if(value==0x11) then
					longpacket = true;
					local hidpp_report = data:subset(offset, 20)
					write_data(hidpp_report)
				end
			end
			
			if(longpacket == false and length >= 7) then
				local offset = length - 7
				local value = data:get_index(offset)
				if(value==0x10) then
					local hidpp_report = data:subset(offset, 7)
					write_data(hidpp_report)
				end
			end
		end
	end

	-- this function will be called whenever a reset is needed
	-- e.g. when reloading the capture file
	function tap.reset()
		tw:clear()
		write_header()
	end

	-- Ensure that all existing packets are processed.
	retap_packets()
end

-- using this function we register our function
-- to be called when the user selects the Tools->Test->Packets menu
register_menu("HID++ data", menuable_tap, MENU_TOOLS_UNSORTED)