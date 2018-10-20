
# ./jpeg_tcm /Users/hossam.amer/7aS7aS_Works/work/my_Tools/jpeg_tcm/dataset/goose.jpg /Users/hossam.amer/7aS7aS_Works/work/my_Tools/jpeg_tcm/QF_exp/ 0

# Number of Parallel tasks
num_parallel_tasks=20

# Define the input path to files
input_path_to_files=/Users/hossam.amer/7aS7aS_Works/work/my_Tools/test_input/tst


# Define output path
output_path_to_files=/Users/hossam.amer/7aS7aS_Works/work/my_Tools/jpeg_tcm/QF_exp/

# jpeg_files="$(ls $input_path_to_files)"
# jpeg_files=(`ls -B "$input_path_to_files")
# jpeg_files=(`ls $input_path_to_files`)
jpeg_files=($input_path_to_files/*)
jpeg_files_count=`ls -B "$input_path_to_files" | wc -l`

# Print the jpeg files and their total number
# echo "${jpeg_files[@]}"
# echo $jpeg_files_count



# Generate quality factors
noQp=11
scaleQpIndex=0
stepSizeQp=10
startQp=0
for ((i = 1; i<=noQp;i++))
do
  Qp[$i]=$(($startQp + $scaleQpIndex*$stepSizeQp))
  let "scaleQpIndex+=1"
  # echo ${Qp[$i]}
done


# Generate the list of commands:

# count the total number of commands
commands_count=0
for (( i = 0; i < jpeg_files_count; i++ ))
do
	current_jpeg=${jpeg_files[$i]}
	# echo $current_jpeg

	# For every YUV, CFG, Case sequence file -> Run All Qps
	for ((j = 1; j<=noQp;j++))
	do
		# cmd="./jpeg_tcm $current_jpeg $output_path_to_files "${Qp[$j]}""
		cmd="./jpeg_tcm $current_jpeg $output_path_to_files "${Qp[$j]}""
    	cmd_array+=("$cmd")
    	let "commands_count+=1"
    	# echo $cmd
	done

	# echo '------------'

done



# Run in Parallel - Sequence level parallelism
# assume one case for now
#num_parallel_tasks=2
start=0
group_id=0
noSeq=$jpeg_files_count

echo 'Total number of commands: ' $commands_count

# Loop
# while [  $start -lt $noSeq ]; do {
while [  $start -lt $commands_count ]; do {

  # end cannot exceed the length of the YUV array
  end=$(($start + $num_parallel_tasks))
  if [ "$(($end))" -gt "$commands_count" ]; then
     end=$commands_count
  fi

 # Start new fork for the next group
 if [ "$(($start))" == "$num_parallel_tasks" ]; then
    group_id=$(($group_id + 1))
    echo -e '\n\n New Group of id $group_id will start now...\n\n'
 fi

  # Run in parallel
  while [  $start -lt $end ]; do {
    cmd="${cmd_array[start]}"
    echo "Process \"$start\" \"$cmd\" started";
    $cmd & pid=$!
    PID_LIST+=" $pid";
    start=$(($start + 1))
  } done

  trap "kill $PID_LIST" SIGINT
  echo "Parallel processes have started";
  wait $PID_LIST
  echo -e "\nAll processes have completed";
} done




#******#******#******#******#******#******#******#******#******#******

