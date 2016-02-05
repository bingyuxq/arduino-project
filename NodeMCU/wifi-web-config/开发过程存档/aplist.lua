wifi.setmode(wifi.STATION)
apCfg={}
staCfg={}

function writeap()
    function listap(t)
        for k,v in pairs(t) do
            print(k.."--"..v)
        end
        collectgarbage()
    end
    wifi.sta.getap(listap)
end

writeap()