wifi.setmode(wifi.STATION)
wifi.sta.config("bingyu-AP","bingyuxq")
print(wifi.sta.getip())
tmr.delay(5000)
led0 = 0
led1 = 1
led2 = 2
led3 = 3
led4 = 4
led5 = 5
led6 = 6
led7 = 7
led9 = 9
led10 = 10
gpio.mode(led0, gpio.OUTPUT)
gpio.mode(led1, gpio.OUTPUT)
gpio.mode(led2, gpio.OUTPUT)
gpio.mode(led3, gpio.OUTPUT)
gpio.mode(led4, gpio.OUTPUT)
gpio.mode(led5, gpio.OUTPUT)
gpio.mode(led6, gpio.OUTPUT)
gpio.mode(led7, gpio.OUTPUT)
gpio.mode(led9, gpio.OUTPUT)
gpio.mode(led10, gpio.OUTPUT)
srv=net.createServer(net.TCP)
srv:listen(80,function(conn)
conn:on("receive", function(client,request)
local buf = ""
local _, _, method, path, vars = string.find(request, "([A-Z]+) (.+)?(.+) HTTP")
if(method == nil)then
_, _, method, path = string.find(request, "([A-Z]+) (.+) HTTP")
end
local _GET = {}
if (vars ~= nil)then
for k, v in string.gmatch(vars, "(%w+)=(%w+)&*") do
_GET[k] = v
end
end
buf = buf.."<h1> ESP8266 Web Server</h1>"
buf = buf.."<p>GPIO16=0 <a href=\"?pin=ON0\"><button>ON</button></a> <a href=\"?pin=OFF0\"><button>OFF</button></a></p>"
buf = buf.."<p>GPIO5=1 <a href=\"?pin=ON1\"><button>ON</button></a> <a href=\"?pin=OFF1\"><button>OFF</button></a></p>"
buf = buf.."<p>GPIO4=2 <a href=\"?pin=ON2\"><button>ON</button></a> <a href=\"?pin=OFF2\"><button>OFF</button></a></p>"
buf = buf.."<p>GPIO0=3 <a href=\"?pin=ON3\"><button>ON</button></a> <a href=\"?pin=OFF3\"><button>OFF</button></a></p>"
buf = buf.."<p>GPIO2=4 <a href=\"?pin=ON4\"><button>ON</button></a> <a href=\"?pin=OFF4\"><button>OFF</button></a></p>"
buf = buf.."<p>GPIO14=5 <a href=\"?pin=ON5\"><button>ON</button></a> <a href=\"?pin=OFF5\"><button>OFF</button></a></p>"
buf = buf.."<p>GPIO12=6 <a href=\"?pin=ON6\"><button>ON</button></a> <a href=\"?pin=OFF6\"><button>OFF</button></a></p>"
buf = buf.."<p>GPIO13=7 <a href=\"?pin=ON7\"><button>ON</button></a> <a href=\"?pin=OFF7\"><button>OFF</button></a></p>"
buf = buf.."<p>GPIO3=9 <a href=\"?pin=ON9\"><button>ON</button></a> <a href=\"?pin=OFF9\"><button>OFF</button></a></p>"
buf = buf.."<p>GPIO1=10 <a href=\"?pin=ON10\"><button>ON</button></a> <a href=\"?pin=OFF10\"><button>OFF</button></a></p>"
buf = buf.."<p>ADC is"..adc.read(0).."</p>"
buf = buf.."<p>node heap left:"..node.heap().."</p>"
local _on,_off = "",""
if(_GET.pin == "ON0")then
gpio.write(led0, gpio.HIGH)
elseif(_GET.pin == "OFF0")then
gpio.write(led0, gpio.LOW)
elseif(_GET.pin == "ON1")then
gpio.write(led1, gpio.HIGH)
elseif(_GET.pin == "OFF1")then
gpio.write(led1, gpio.LOW)
elseif(_GET.pin == "ON2")then
gpio.write(led2, gpio.HIGH)
elseif(_GET.pin == "OFF2")then
gpio.write(led2, gpio.LOW)
elseif(_GET.pin == "ON3")then
gpio.write(led3, gpio.HIGH)
elseif(_GET.pin == "OFF3")then
gpio.write(led3, gpio.LOW)
elseif(_GET.pin == "ON4")then
gpio.write(led4, gpio.HIGH)
elseif(_GET.pin == "OFF4")then
gpio.write(led4, gpio.LOW)
elseif(_GET.pin == "ON5")then
gpio.write(led5, gpio.HIGH)
elseif(_GET.pin == "OFF5")then
gpio.write(led5, gpio.LOW)
elseif(_GET.pin == "ON6")then
gpio.write(led6, gpio.HIGH)
elseif(_GET.pin == "OFF6")then
gpio.write(led6, gpio.LOW)
elseif(_GET.pin == "ON7")then
gpio.write(led7, gpio.HIGH)
elseif(_GET.pin == "OFF7")then
gpio.write(led7, gpio.LOW)
elseif(_GET.pin == "ON9")then
gpio.write(led9, gpio.HIGH)
elseif(_GET.pin == "OFF9")then
gpio.write(led9, gpio.LOW)
elseif(_GET.pin == "ON10")then
gpio.write(led10, gpio.HIGH)
elseif(_GET.pin == "OFF10")then
gpio.write(led10, gpio.LOW)
end
client:send(buf)
client:close()
collectgarbage()
end)
end)