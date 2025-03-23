

for AAA in "0.1"  "0.9"
do
	dir1="p1_"$AAA
	cd $dir1
	for BBB in "0.1"  "0.9"
	do
		dir2="p2_"$BBB
		cd $dir2
			
		for CCC in "0" "0.3"  "0.7" "0.8" "0.9" "1.0"
			
		do
			dir3="beta_"$CCC		
			cd $dir3
			
			cat ./results.txt

			cd ..

		done
		cd ..
	done
	cd ..
done
