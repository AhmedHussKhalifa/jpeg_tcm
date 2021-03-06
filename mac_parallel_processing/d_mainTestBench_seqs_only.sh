
# ./jpeg_tcm /Users/hossam.amer/7aS7aS_Works/work/my_Tools/jpeg_tcm/dataset/goose.jpg /Users/hossam.amer/7aS7aS_Works/work/my_Tools/jpeg_tcm/QF_exp/ 0

# Number of Parallel tasks (make sure that they are multiples of 11 since we are running 11 QF per image)
num_parallel_tasks=32

# Define the input path to files
 input_path_to_files=/Users/hossam.amer/7aS7aS_Works/work/my_Tools/test_input/tst
#input_path_to_files=/Volumes/DATA/ml/ImageNet_2nd


# Define output path
 output_path_to_files=/Users/hossam.amer/7aS7aS_Works/work/my_Tools/jpeg_tcm/QF_exp/
#output_path_to_files=/Volumes/DATA/ml/imageNet_output_folder/

# mkdir output path
mkdir $output_path_to_files

echo 'Processing JPEG files'
# jpeg_files="$(ls $input_path_to_files)"
# jpeg_files=(`ls -B "$input_path_to_files")
jpeg_files=(`ls $input_path_to_files`)
# jpeg_files=($input_path_to_files/*)
jpeg_files_count=`ls -B "$input_path_to_files" | wc -l`

# Print the jpeg files and their total number
# echo "${jpeg_files[@]}"
# echo $jpeg_files_count

if [ "$num_parallel_tasks" -gt "$jpeg_files_count" ]; then
let "num_parallel_tasks=$jpeg_files_count-1"
fi 


# Generate quality factors
scaleQpIndex=0
stepSizeQp=10
startQp=0



# Generate the list of commands:


echo 'Generating Commands list and running batches of images'

# count the total number of commands
commands_count=0


# parallel group id
group_id=0


for (( i = 0; i < jpeg_files_count; i++ ))
do
	current_jpeg_help=${jpeg_files[$i]}
	current_jpeg=$input_path_to_files/$current_jpeg_help
	#echo $current_jpeg

	# For every YUV, CFG, Case sequence file -> Run All Qps
	cmd="./jpeg_tcm $current_jpeg $output_path_to_files 100"
    	cmd_array+=("$cmd")
    	let "commands_count+=1"
    	echo $cmd

	# echo '------------'

  # echo 'Commands Count:' $commands_count
  if [ "$(($commands_count))" == "$num_parallel_tasks" ]; then


    # Run in Parallel - Sequence level parallelism
    group_id=$(($group_id + 1))
    echo -e '\n\n New Group of id' $group_id 'will start now...\n\n'

    # start of the commands list is always zero
    start=0

    # end cannot exceed the length of the YUV array
    end=$(($start + $num_parallel_tasks))
    if [ "$(($end))" -gt "$num_parallel_tasks" ]; then
       end=$num_parallel_tasks
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

  # reset the commands count
  commands_count=0
  # shift your commands array to the left for the tasks you already ran
  cmd_array=("${cmd_array[@]:$num_parallel_tasks}")

  # shift your PID list (clear it)
  PID_LIST=("${PID_LIST[@]:$num_parallel_tasks}")

fi

done # done for loop



echo 'Total number of jpeg files processed: ' $jpeg_files_count
echo 'Total number of groups processed: ' $group_id
# echo 'Total number of commands executed: ' $commands_count

#******#******#******#******#******#******#******#******#******#******

