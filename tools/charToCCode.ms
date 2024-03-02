// Little utility script to run in Mini Micro, to spit out
// font data in the form of C/C++ code.

import "chars"
output = []
TAB = char(9)

charToCCode = function(c)
	gfx.clear color.black
	gfx.scale = 10
	gfx.print c, 0, 0, color.white
	
	cw = 16
	ch = 24
	
	output.push TAB + "uint32_t charData_" + c.code + "[] = {"
	for y in range(ch-1, 0)
		bits = []
		for x in range(0, cw)
			p = gfx.pixel(x, y)
			if p[1] == "F" then bits.push 1 else bits.push 0
		end for
		output.push TAB*2 + "0b" + bits.join("") + ","*(y>0)
	end for
	output.push TAB + "};"
	output.push TAB + "lowCharData[" + c.code + "-32] = charData_" + c.code + ";"
	output.push ""
end function

clear
for c in chars.allPrintable
	charToCCode c
end for
path = "fontData.cpp"
file.writeLines path, output
print "Wrote data to " + path


