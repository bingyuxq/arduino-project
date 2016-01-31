-- 连接wifi
wifi.setmode(wifi.STATION)  
wifi.sta.config("bingyu-AP","bingyuxq")  
print(wifi.sta.getip())  
--end

-- 定义BMP180压力温度传感器
OSS = 1 -- oversampling setting (0-3)
SDA_PIN = 4 -- sda pin, GPIO2
SCL_PIN = 3 -- scl pin, GPIO0
bmp180 = require("bmp180")
bmp180.init(SDA_PIN, SCL_PIN)
--end

-- 定义继电器
led1 = 1  --GPIO5
led2 = 7  --GPIO13
gpio.mode(led1, gpio.OUTPUT)  
gpio.mode(led2, gpio.OUTPUT)  
--end

-- 定义SR501人体热信号传感器,这个信号定义为中断，发现变化后立即更新状态
SR501pin = 6  --GPIO12
gpio.mode(SR501pin,gpio.INT)

--end

-- -- 创建TCP相应服务器作为网页服务器
-- srv=net.createServer(net.TCP)  
-- srv:listen(80,function(conn)  
    -- conn:on("receive", function(client,request)  

        -- local buf = "";  
        -- local _, _, method, path, vars = string.find(request, "([A-Z]+) (.+)?(.+) HTTP");  
        -- if(method == nil)then  
            -- _, _, method, path = string.find(request, "([A-Z]+) (.+) HTTP");  
        -- end  
		
		-- -- 获取HTTP交互数据
        -- local _GET = {}  
        -- if (vars ~= nil)then  
            -- for k, v in string.gmatch(vars, "(%w+)=(%w+)&*") do  
                -- _GET[k] = v  
            -- end  
        -- end  
  
		-- -- 读取传感器数据
		-- bmp180.read(OSS) --BMP180
		-- t = bmp180.getTemperature()/10 --温度
		-- p = bmp180.getPressure() --大气压
		-- d = gpio.read(SR501pin) --人体感应

		-- -- 数据回显
        -- buf = buf.."<h1> ESP8266 Web Server</h1>";  
        -- buf = buf.."<p>".."Temperature: "..(t).." deg C".."</p>";
		-- buf = buf.."<p>".."Pressure: "..(p).." Pa".."</p>";
		-- buf = buf.."<p>".."Persion: "..(d).."</p>";
        -- buf = buf.."<p>GPIO0 <a href=\"?pin=ON1\"><button>ON</button></a> <a href=\"?pin=OFF1\"><button>OFF</button></a></p>";  
        -- buf = buf.."<p>GPIO2 <a href=\"?pin=ON2\"><button>ON</button></a> <a href=\"?pin=OFF2\"><button>OFF</button></a></p>";  
        -- buf = buf.."<p>ADC is"..adc.read(0).."</p>";
		
		-- -- 相应开关信号
        -- local _on,_off = "",""  
        -- if(_GET.pin == "ON1")then  
              -- gpio.write(led1, gpio.HIGH);  
        -- elseif(_GET.pin == "OFF1")then  
              -- gpio.write(led1, gpio.LOW);  
        -- elseif(_GET.pin == "ON2")then  
              -- gpio.write(led2, gpio.HIGH);  
        -- elseif(_GET.pin == "OFF2")then  
              -- gpio.write(led2, gpio.LOW);  
        -- end  
		
		-- -- 发送HTTP数据
        -- client:send(buf);  
        -- client:close();

		-- -- 垃圾回收
        -- collectgarbage();  
    -- end)  
-- end)  


-- 创建一个TCP连接
socket=net.createConnection(net.TCP, 0)

-- 域名解析IP地址并赋值
socket:dns("www.tlink.io", function(conn, ip)
	ServerIP = ip
	--print("Connection IP:" .. ServerIP)
	end)
socket = nil

-- 创建一个定时器用于上传数据
tmr.alarm(0, 5000, 1, function()

	-- 读取传感器数据
	bmp180.read(OSS)
	t = bmp180.getTemperature()/10
	p = bmp180.getPressure()/1000
	d = gpio.read(SR501pin)
	
	-- 定义数据变量格式
	PostData = "{\"deviceNo\":\"XA44OZN9JFA7AP3C\",\"sensorDatas\":["
	PostData = PostData.."{\"sensorsId\":13122,\"value\":\""..(t).."\"},"	-- 温度数据
	PostData = PostData.."{\"sensorsId\":13123,\"value\":\""..(p).."\"},"	-- 大气压数据
	PostData = PostData.."{\"sensorsId\":13124,\"switcher\":\""..(d).."\"}"	-- 人体感应数据
	PostData = PostData.."]}"
	
	-- 创建一个TCP连接
	socket=net.createConnection(net.TCP, 0)
			
	-- 获取执行结果HTTP响应内容
	--socket:on("receive", function(sck, response)
	--	print(response)
	--	end)	
		
	-- 开始连接服务器
	socket:connect(80, ServerIP)
	--socket:on("connection", function(sck) end)
	
	-- HTTP请求头定义
	socket:send("POST http://www.tlink.io/tlink_interface/api/device/createDataPonit.htm HTTP/1.1\r\n" ..
				"Host: www.tlink.io\r\n" ..
				"Content-Length: " .. string.len(PostData) .. "\r\n\r\n" ..
				PostData .. "\r\n")
	print(node.heap())
	socket = nil
	collectgarbage();
end)


-- 创建一个定时器用于下载数据
-- tmr.alarm(1, 5000, 1, function()

	-- -- 定义需要控制的接口
	-- led1 = 1  --GPIO5
	-- led2 = 7  --GPIO13
	-- gpio.mode(led1, gpio.OUTPUT)  
	-- gpio.mode(led2, gpio.OUTPUT)
	
	-- -- 创建一个TCP连接
	-- socket=net.createConnection(net.TCP, 0)
	
	-- -- 域名解析IP地址并赋值
	-- socket:dns("www.tlink.io", function(conn, ip)
		-- ServerIP = ip
		-- --print("Connection IP:" .. ServerIP)
		-- end)
		
	-- -- 开始连接服务器
	-- socket:connect(80, ServerIP)
	-- socket:on("connection", function(sck) end)
	
	-- -- HTTP请求头定义
	-- socket:send("POST http://www.tlink.io/tlink_interface/api/device/getDataPoint_13125.htm HTTP/1.1\r\n" ..
				-- "Host: www.tlink.io\r\n" ..
				-- "Content-Length: 0\r\n\r\n")
				
	-- -- 获取执行结果HTTP响应内容
	-- socket:on("receive", function(sck, response)
			-- print(response)
			-- if string.find(response,"\"switcher\":\"0\"") then
				-- gpio.write(led1, gpio.LOW); 
				-- print("LOW")
			-- else
				-- gpio.write(led1, gpio.HIGH); 
			-- print("HIGH")
			-- end
		-- end)	
	-- print(node.heap())
	-- collectgarbage();
-- end)