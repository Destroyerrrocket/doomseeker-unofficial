-- Automatically exeucted from makemacdmg.sh
-- Not to be run alone.

on run
	tell application "Finder"
		tell disk "Doomseeker"
			open
			set options to icon view options of container window
			tell options
					set icon size to 96
					set arrangement to not arranged
			end tell
			set background picture of options to file ".background:background.png"
			tell container window
				set the bounds to {0, 0, 500, 350}
				set current view to icon view
				set toolbar visible to false
				set statusbar visible to false
			end tell

			make new alias to "Macintosh HD:Applications" at "Doomseeker" with properties {name:"Applications"}

			set position of item "Doomseeker.app" to {75, 150}
			set label index of item "Doomseeker.app" to 7
			set position of item "Applications" to {425, 150}
			set label index of item "Applications" to 7
			close
		end tell
	end tell
end run
