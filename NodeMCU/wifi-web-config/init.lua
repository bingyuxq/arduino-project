local trys = 0
APlist = ""

function failsafe()
	print("ops!init failed")
	dofile("aplist.lua")
	tmr.alarm(2, 10000, 0 ,function()	-- �ӳ�10������ڻ�ȡ�ȵ��б�
		dofile("setup.lua")
	end)
end

function payload()
	print("payload loaded")
end

if (not pcall(function()require("wifi-config")end)) then
	failsafe()
else
	wifi.setmode (wifi.STATION)
	wifi.sta.config(SSID, SSID_PASSWORD)
	--wifi.sta.sethostname("BINGYU_IOT_"..node.chipid())	��ǰ�汾�ݲ�����
	wifi.sta.autoconnect (1)
	-- ��������Ԥ����wifi
	tmr.alarm (2, 500, 1, function()
	  if (wifi.sta.getip() == nil) then
		 print ("Waiting for Wifi connection."..30-trys)
		 trys = trys + 1
		 if (trys >10) then
			print ("Time out,errcode is:"..wifi.sta.status())
			tmr.stop (2)
			failsafe()
		 end
	  else
		 tmr.stop (2)
		 print ("Config done, IP is " .. wifi.sta.getip())
		 payload()
	  end
	end)
end