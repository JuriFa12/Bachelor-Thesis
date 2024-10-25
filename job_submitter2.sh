#!/bin/bash


job_folder="/home/farrukujur/tesi/job_files"

job_name="job_array"

sbatch_options="--wckey=rop --requeue"

for index in {1..240}; do
	 sbatch $sbatch_options $job_folder/job_${index}.sh
done


