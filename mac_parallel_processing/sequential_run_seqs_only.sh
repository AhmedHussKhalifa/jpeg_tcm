
# ./jpeg_tcm /Users/hossam.amer/7aS7aS_Works/work/my_Tools/jpeg_tcm/dataset/goose.jpg /Users/hossam.amer/7aS7aS_Works/work/my_Tools/jpeg_tcm/QF_exp/ 0

# Number of Parallel tasks (make sure that they are multiples of 11 since we are running 11 QF per image)
num_parallel_tasks=200

# for (( folder = 1; folder < 126; folder++ ))
# do
	
# Define the input path to files
# input_path_to_files=/Users/hossam.amer/Desktop/Projects/ML_TS/training_set_500/$folder
input_path_to_files=/Volumes/DATA/ml/test1
# input_path_to_files=/Volumes/DATA/ml/ImageNet_2nd
# input_path_to_files=/Users/hossam.amer/7aS7aS_Works/work/my_Tools/jpeg_tcm/dataset/tcm_analysis
# input_path_to_files=/Users/hossam.amer/7aS7aS_Works/work/my_Tools/jpeg_tcm/dataset/set/set1


# Define output path
# output_path_to_files=/Users/hossam.amer/7aS7aS_Works/work/my_Tools/jpeg_tcm/QF_exp/
# output_path_to_files=/Users/hossam.amer/Desktop/Projects/ML_TS/train_output_folder/$folder/
# output_path_to_files=/Users/hossam.amer/7aS7aS_Works/work/my_Tools/jpeg_tcm/dataset/out_tcm_analysis/
output_path_to_files=/Volumes/DATA/ml/out_test/



# jpeg_files="$(ls $input_path_to_files)"
# jpeg_files=(`ls -B "$input_path_to_files")
jpeg_files=(`ls $input_path_to_files`)
# jpeg_files=($input_path_to_files/*)
jpeg_files_count=`ls -B "$input_path_to_files" | wc -l`

echo 'Processing JPEG files: ' $jpeg_files_count

# Print the jpeg files and their total number
# echo "${jpeg_files[@]}"
# echo $jpeg_files_count



# Generate quality factors
noQp=11
scaleQpIndex=0
stepSizeQp=10
startQp=0

# echo 'Generating Quality factors'
# for ((i = 1; i<=noQp;i++))
# do
#   Qp[$i]=$(($startQp + $scaleQpIndex*$stepSizeQp))
#   let "scaleQpIndex+=1"
#   # echo ${Qp[$i]}
# done


# Generate the list of commands:


echo 'Generating Commands list and running batches of images'

# count the total number of commands
commands_count=0


# parallel group id
group_id=0


let "count_helper=$jpeg_files_count-1"
for (( i = 0; i < jpeg_files_count; i++ ))
do
  current_jpeg_help=${jpeg_files[$i]}
  current_jpeg=$input_path_to_files/$current_jpeg_help
  #echo $current_jpeg

  # For every YUV, CFG, Case sequence file -> Run All Qps
  ./jpeg_tcm_no_cv $current_jpeg $output_path_to_files 100
done # done for loop



# echo 'Total number of QP factors processed: ' $noQp
echo 'Total number of jpeg files processed: ' $jpeg_files_count
echo 'Total number of groups processed: ' $group_id
# echo 'Total number of commands executed: ' $commands_count


# done # folder loop
#******#******#******#******#******#******#******#******#******#******

