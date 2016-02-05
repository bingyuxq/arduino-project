wifi.setmode(wifi.STATION)
APlist ="<table border='1'><tr><td>WiFi名称</td><td>加密方式</td><td>信号强度</td><td>网关MAC地址</td><td>信道</td></tr>"

function split(szFullString, szSeparator)  
	local nFindStartIndex = 1  
	local nSplitIndex = 1  
	local nSplitArray = {}  
	while true do  
		local nFindLastIndex = string.find(szFullString, szSeparator, nFindStartIndex)  
		if not nFindLastIndex then  
			nSplitArray[nSplitIndex] = string.sub(szFullString, nFindStartIndex, string.len(szFullString))  
			break  
		end  
		nSplitArray[nSplitIndex] = string.sub(szFullString, nFindStartIndex, nFindLastIndex - 1)  
		nFindStartIndex = nFindLastIndex + string.len(szSeparator)  
		nSplitIndex = nSplitIndex + 1  
	end
	return nSplitArray  
end  

function listap(t)
	for k,v in pairs(t) do
		v = split(v,",")
		APlist = APlist.."<tr><td>"..k.."</td><td>"..v[1].."</td><td>"..v[2].."</td><td>"..v[3].."</td><td>"..v[4].."</td></tr>\r\n"
	end
	APlist = APlist.."</table><br />"
	collectgarbage()
end

wifi.sta.getap(listap)