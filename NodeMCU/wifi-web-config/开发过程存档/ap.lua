local trys = 0
local staip = nil

function failsafe()
	-- 任何原因连接AP失败重新创建配置用接入点
		print("ops!failed!")
		
		-- AP configuration
		apcfg={}
		ipcfg={}
		apcfg.ssid="LAVA_LAMP_"..node.chipid()
--		apcfg.pwd=""
		ipcfg.ip = "192.168.4.1"
		ipcfg.netmask = "255.255.255.0"
		ipcfg.gateway = "192.168.4.1"

		--create AP
		wifi.setmode(wifi.SOFTAP)
		wifi.ap.config(apcfg)
		wifi.ap.setip(ipcfg)
		print(wifi.ap.getip())
		staip = wifi.sta.getip()
		-- get AP IP, don't know why, but without it everything breaks
		--tmr.alarm(2,5000,0, wifi.ap.getip)	--等待5s获取ip地址
end


if (not pcall(function()require("wifi-config")end)) then
	if(SSID ~= nil) then
		wifi.setmode (wifi.STATION)
		wifi.sta.config(SSID, SSID_PASSWORD)
		wifi.sta.sethostname("LAVA_LAMP_"..node.chipid())
		wifi.sta.autoconnect (1)
		print(wifi.sta.getip())

		-- 尝试连接预配置wifi
		tmr.alarm (2, 1000, 1, function()
		  if (wifi.sta.getip() == nil) then
			 print ("Waiting for Wifi connection")
			 trys = trys + 1
			 if (trys >30) then
				print ("Time out,errcode is:"..wifi.sta.status())
				tmr.stop (2)
				failsafe()
			 end
		  else
			 tmr.stop (2)
			 print ("Config done, IP is " .. wifi.sta.getip())
		  end
		end)
	else
		failsafe()
	end
else
	 print ("Config loaded, IP is " .. wifi.sta.getip())
end


--create HTTP server
local srv=net.createServer(net.TCP)
srv:listen(80,function(conn)
	conn:on("receive",function(conn,payload)
		local ssid,password
		print(payload)
		--webpage header
		conn:send("<!DOCTYPE html><html lang='en'><body><h1>Wireless Lava Lamp setup</h1><br/>")
		print(wifi.sta.status())
		--parse GET response
		local parameters = string.match(payload, "^GET(.*)HTTP\/1.1")
		if parameters then
			ssid = string.match(parameters, "SSID=([a-zA-Z0-9+-_]+)")
			password = string.match(parameters, "PASS=([a-zA-Z0-9+]+)")
		end
		if ssid then
			--wait for 30 seconds and refresh webpage (wait for IP)
			conn:send([[<script type='text/javascript'>
				var timeout = 30;window.onload=function(){function countdown() {
				if ( typeof countdown.counter == 'undefined' ) {countdown.counter = timeout;}
				if(countdown.counter > 0){document.getElementById('count').innerHTML = countdown.counter
				else {location.href = 'http://]]..wifi.ap.getip()..[[';};};countdown();};
				</script><h2>Autoconfiguration will end in <span id='count'></span> seconds</h2></body></html>]])
			conn:close()
			ssid = string.gsub(ssid,'+',' ')
			--print("ssid: '"..ssid.."' password: '"..password.."'")
			wifi.setmode(wifi.STATIONAP)
			-- configure the module so it can connect to the network using the received SSID and password
			wifi.sta.config(ssid,password)
			wifi.sta.autoconnect(1)
			trys = 0
			--wait for IP
			tmr.alarm (1, 500, 1, function ()
				if wifi.sta.getip () ~= nil then
					tmr.stop(1)
					staip = wifi.sta.getip()
					print("Config done, IP is " .. staip)
					if(ssid ~=nil) then
						file.open("wifi-config.lua","w+")
						file.writeline("SSID = \""..ssid.."\"")
						if(password == nil) then
							password = ""
						end
						file.writeline("SSID_PASSWORD = \""..password.."\"")
						file.flush()
						file.close()
					end
				end
				if trys > 50 then
					print("Cannot connect to AP")
					tmr.stop(1)
				end
				print(trys)
				trys = trys + 1
			end)
		else
			--Print error and retry
			if trys > 50 then
				if (wifi.sta.status() == 2) then
					conn:send("<h2 style='color:red'>Wrong network password, try again</h2>")
				elseif (wifi.sta.status() == 3) then
					conn:send("<h2 style='color:red'>Could not find network, try again</h2>")
				else
					conn:send("<h2 style='color:red'Cannot connect to network, try again</h2>")
				end
			end
			--Main configuration web page
			conn:send([[<h2>The module MAC address is: ]].. wifi.ap.getmac()..[[</h2>
<h2>Enter SSID and Password for your WIFI router</h2>

<form action='' method='get' accept-charset='ascii'>
SSID:
<input type='text' name='SSID' value='' maxlength='32' placeholder='your network name'/>
<br />
Password:
<input type='text' name='PASS' value='' maxlength='100' placeholder='network password'/>
<input type='submit' value='Submit' />
</form> </body> </html>]])
			conn:close()
		end
	end)
	conn:on("sent",function(conn) conn:close() end)
end)

-- 10分钟后关闭web服务器
tmr.alarm (2, 600000, 0, function()
	--srv.server.close()
	srv = nil
	print("10 min passed,webserver shutdown!")
	collectgarbage()
end)