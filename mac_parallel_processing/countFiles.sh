
input_path=/Volumes/DATA/ml/rename_test/ 
gen_path=/Volumes/DATA/ml/rename_test/

# Count1
i=1
for D in $input_path*; do
    if [ -d "${D}" ]; then
        #echo "${D}"   # your processing here
       	count1[$i]=`ls -B "$D" | wc -l`
       	#echo ${count1[$i]}
	    let i=i+1
    fi
done

# Count2
j=1
for D in $gen_path*; do
    if [ -d "${D}" ]; then
        #echo "${D}"   # your processing here
       	count2[$j]=`ls -B "$D" | wc -l`
       	#echo ${count2[$j]}
	    let j=j+1
    fi
done

# Display
echo '-----DISPLAY COUNTS-----------'
for ((k = 1; k<i;k++))
do

# Get org
orgCount=${count1[$k]}	
let "orgCount=orgCount*11"

#Get generated
genCount=${count2[$k]}

echo $k')' $orgCount '<--->' $genCount
done

echo '-----------------------'