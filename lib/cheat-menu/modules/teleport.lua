-- This module contains all the functions related to teleportation tab

local module = {}

-- Teleport table
local tteleport =
{
    shortcut       = imgui.new.bool(fconfig.get('tteleport.shortcut',false)),
    coords         = imgui.new.char[24](fconfig.get('tteleport.coords',"")),
    auto_z         = imgui.new.bool(fconfig.get('tteleport.auto_z',false)),
    insert_coords  = imgui.new.bool(fconfig.get('tteleport.insert_coords',false)),
    search_text    = imgui.new.char[64](fconfig.get('tteleport.search_text',"")),
}

module.tteleport = tteleport


local coordinates = ftables.coordinates.table


function module.Teleport(x, y, z,interior_id)
	if x == nil then
		_, x,y,z = getTargetBlipCoordinates()
		interior_id = 0
	end
	if tteleport.auto_z[0] then
		z = getGroundZFor3dCoord(x, y, z)
	end

	setCharInterior(PLAYER_PED,interior_id)
	setInteriorVisible(interior_id)
	clearExtraColours(true)
	requestCollision(x,y)
	activateInteriorPeds(true)
	setCharCoordinates(PLAYER_PED, x, y, z)
	loadScene(x,y,z)

end


function ShowTeleportEntry(label, x, y, z,interior_id)
	if imgui.MenuItemBool(label) then
		module.Teleport(x, y, z,interior_id)
	end
end

function module.TeleportMain()

    if imgui.BeginTabBar("Teleport") then
        if imgui.BeginTabItem(flanguage.GetText("teleport.Teleport")) then
            imgui.Spacing()
            imgui.Columns(2,nil,false)
            fcommon.CheckBox({name = flanguage.GetText("teleport.AutoZCoordinates"),var = fteleport.tteleport.auto_z,help_text =flanguage.GetText("teleport.AutoZCoordinatesToolTip")})
			fcommon.CheckBox({name = flanguage.GetText("teleport.InsertCoordinates"),var = fteleport.tteleport.insert_coords,help_text =flanguage.GetText("teleport.InsertCoordinatesToolTip")})
			imgui.NextColumn()
			fcommon.CheckBox({name = flanguage.GetText("teleport.QuickTeleport"),var = fteleport.tteleport.shortcut,help_text =flanguage.GetText("teleport.QuickTeleportToolTip")})
            imgui.Columns(1)

            if imgui.InputText(flanguage.GetText("teleport.Coordinates"),tteleport.coords,ffi.sizeof(tteleport.coords)) then end

            if tteleport.insert_coords[0] then
                local x,y,z = getCharCoordinates(PLAYER_PED)
                imgui.StrCopy(tteleport.coords,string.format("%d, %d, %d", math.floor(x) , math.floor(y) , math.floor(z)))
            end

            fcommon.InformationTooltip(flanguage.GetText("teleport.InputToolTip"))
            imgui.Dummy(imgui.ImVec2(0,10))

            if imgui.Button(flanguage.GetText("teleport.TeleportToCoord"),imgui.ImVec2(fcommon.GetSize(2))) then
                local x,y,z = (ffi.string(tteleport.coords)):match("([^,]+),([^,]+),([^,]+)")
                module.Teleport(x, y, z,0)
            end
            imgui.SameLine()
            if imgui.Button(flanguage.GetText("teleport.TeleportToMarker"),imgui.ImVec2(fcommon.GetSize(2))) then
                module.Teleport()
            end
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem('Search') then
            imgui.Spacing()
            imgui.Columns(1)
            if imgui.InputText(flanguage.GetText("common.Search"),tteleport.search_text,ffi.sizeof(tteleport.search_text)) then end
			imgui.SameLine()

			imgui.Spacing()
			imgui.Text(flanguage.GetText("common.FoundEntries") .. ":(" .. ffi.string(tteleport.search_text) .. ")")
			imgui.Separator()
			imgui.Spacing()
			if imgui.BeginChild("Teleport entries") then
				for name, coord in pairs(coordinates) do
					local interior_id, x, y, z = coord:match("([^, ]+), ([^, ]+), ([^, ]+), ([^, ]+)")
					if ffi.string(tteleport.search_text) == "" then
						ShowTeleportEntry(name, tonumber(x), tonumber(y), tonumber(z),interior_id)
					else
						if string.upper(name):find(string.upper(ffi.string(tteleport.search_text))) ~= nil  then
							ShowTeleportEntry(name, tonumber(x), tonumber(y), tonumber(z),interior_id)
						end
					end
				end
				imgui.EndChild()
			end
            imgui.EndTabItem()
        end
        imgui.EndTabBar()
    end
end

return module
