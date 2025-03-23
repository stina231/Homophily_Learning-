dir0="code"
for AAA in "0.1"  "0.9"
do

	dir1="p1_"$AAA
	rm -rf $dir1
	mkdir $dir1
	cd $dir1
	for BBB in "0.1"  "0.9"
	do
		dir2="p2_"$BBB
		mkdir $dir2
		cd $dir2
		for CCC in  "0" "0.3"  "0.7" "0.8" "0.9" "1.0" 
		do
			dir3="beta_"$CCC		
			mkdir $dir3
			cd $dir3
			cp ../../../$dir0/a.out .
			cp ../../../$dir0/input.dat .

            		cat input.dat|sed "s/AAA/$AAA/g" > a.dat
			cat a.dat|sed "s/BBB/$BBB/g" > b.dat
         	   	cat b.dat|sed "s/CCC/$CCC/g" > t.dat
            		mv -f t.dat input.dat
           	 	./a.out < input.dat
            		rm a.dat
			rm b.dat
			rm a.out
			cd ..	
		done
		cd ..
	done
	cd ..
done
