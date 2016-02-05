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
gpio.trig(SR501pin,"both",function(level)
	-- 定义数据变量格式
	PostData = "{\"deviceNo\":\"XA44OZN9JFA7AP3C\",\"sensorDatas\":["
	PostData = PostData.."{\"sensorsId\":13124,\"switcher\":\""..(level).."\"}"	-- 人体感应数据
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
	print("heap size"..node.heap())
	print("发现入侵！")
	socket = nil
end)

-- 创建一个TCP连接
socket=net.createConnection(net.TCP, 0)

-- 域名解析IP地址并赋值
socket:dns("www.tlink.io", function(conn, ip)
	ServerIP = ip
	--print("Connection IP:" .. ServerIP)
	end)
socket = nil

-- 创建一个定时器用于上传数据
tmr.alarm(0, 60000, 1, function()

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
	print("heap size"..node.heap())
	print("提交更新数据")
	socket = nil
	--collectgarbage();
end)
