wifi.setmode(wifi.SOFTAP)
cfg={}
cfg.ssid="bingyu_IOT_"..node.chipid()
--cfg.pwd=""
cfg.ip = "192.168.4.1"
cfg.netmask = "255.255.255.0"
cfg.gateway = "192.168.4.1"
wifi.ap.config(cfg)

srv=net.createServer(net.TCP)   --make http server
srv:listen(80,function(conn) 
    conn:on("receive",function(conn,HTMLpayload) 
		--print("HTMLpayload "..HTMLpayload)
		if string.find(HTMLpayload,"ssid=") then
			local startssid
			startssid = string.find(HTMLpayload, "ssid=") + 5 
			local endssid  = string.find(HTMLpayload, "&pass=") - 1
			ssid = string.sub(HTMLpayload, startssid, endssid)  --look for ssid in http call
			--print("newssid "..ssid)  --prints to serial, for testing mostly
			--end
			if string.find(HTMLpayload,"pass=") then
				local startpass
				startpass = string.find(HTMLpayload, "pass=") + 5
				local endpass  = string.find(HTMLpayload, "&", startpass) - 1
				password = string.sub(HTMLpayload, startpass, endpass)  --look for password in http call
				--print("newpass "..password)  --prints to serial, for testing
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
				conn:send([[<!DOCTYPE html><html lang='en'><body><h1>bingyu的IOT无线配置器</h1><br/>
							<h2>设置已保存，配置热点将关闭</h2>
							</body> </html>]])
				conn:close()
				srv:close()
			end
			tmr.alarm(1, 5000, 0, function() node.restart() end)
		else
			conn:send([[<!DOCTYPE html><html lang='en'><body><h1>bingyu的IOT无线配置器</h1><br/>
						<h2>本设备的 MAC 地址为: ]].. wifi.ap.getmac()..[[</h2>
						<h2>输入需要连接的WiFi名称和密码来连接</h2>
						<h2>连接成功后将会保存配置并自动重启，如果失败无线配置器将再次启动</h2>
						]]..APlist..[[
						<form action='' method='get' accept-charset='ascii'>
						名称:
						<input type='text' name='ssid' value='' maxlength='32' placeholder='WiFi SSID'/>
						<br />
						密码:
						<input type='text' name='pass' value='' maxlength='100' placeholder='WiFi密码'/>
						<input type='hidden' name='end' value='*'> 
						<input type='submit' value='连接' />
						</form> </body> </html>]])
			conn:close()
		end
    end) 
end)