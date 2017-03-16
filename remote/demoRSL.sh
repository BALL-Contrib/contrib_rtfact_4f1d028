echo "RUNNING SHADER DEMO..."

# teapot_box_vp1: shd0 = teapot, shd1 = box
# teapot_box_vp2: shd0 = teapot, shd1 = box
# teapot_box_vp3: shd0 = teapot, shd1 = box

# sphere_box_vp1: shd0 = sphere, shd1 = box
# sphere_box_vp2: shd0 = sphere, shd1 = box

# box: shd0 = box

rtsgRTremoteTest scenes/AnySL_Columns_RSL.wrl
rtsgRTremoteTest scenes/AnySL_Columns_RSL_pkt.wrl
rtsgRTremoteTest scenes/AbstractSciFi.wrl
rtsgRTremoteTest scenes/TVSet_Final3.wrl

./render_wrl.sh scenes/teapot_box_vp1.x3dv -s rsl_glass -s rsl_checker
./render_wrl.sh scenes/teapot_box_vp1.x3dv -s rsl_glass -s rsl_checker -p

./render_wrl.sh scenes/sphere_box_vp2.x3dv -s rsl_cyclone -s rsl_wood
#./render_wrl.sh scenes/sphere_box_vp2.x3dv -s rsl_cyclone -s rsl_wood -p #currently packetization of cyclone hangs ;)

./render_wrl.sh scenes/teapot_box_vp2.x3dv -s rsl_plastic -s rsl_glass
./render_wrl.sh scenes/teapot_box_vp2.x3dv -s rsl_plastic -s rsl_glass -p

./render_wrl.sh scenes/teapot_box_vp3.x3dv -s rsl_whitted -s rsl_brick
./render_wrl.sh scenes/teapot_box_vp3.x3dv -s rsl_whitted -s rsl_brick -p

./render_wrl.sh scenes/teapot_box_vp4.x3dv -s rsl_mirror -s rsl_metal
./render_wrl.sh scenes/teapot_box_vp4.x3dv -s rsl_mirror -s rsl_metal -p

./render_wrl.sh scenes/sphere_box_vp1.x3dv -s rsl_screen -s rsl_granite
./render_wrl.sh scenes/sphere_box_vp1.x3dv -s rsl_screen -s rsl_granite -p

./render_wrl.sh ../../../../models/wrl/f030.wrl -s rsl_checker
./render_wrl.sh ../../../../models/wrl/f030.wrl -s rsl_checker -p

./render_wrl.sh ../../../../models/wrl/bun_zipper.wrl -s rsl_plastic -p

#./render_wrl.sh ../../../../models/wrl/dragon_vrip.wrl -s rsl_brick -p

#./render_wrl.sh ../../../../models/wrl/happy_vrip.wrl -s rsl_wood -p



echo ""
echo "SHADER DEMO FINISHED!"
echo ""
