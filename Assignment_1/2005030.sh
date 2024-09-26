#!/usr/bin/bash

if [ $# -eq 0 ]; then
echo "No input.txt file given."
exit 1
fi

#Processing input file
file_path="$1"
lines=()

while IFS= read -r line; do
    lines+=("$line")
    echo "$line"
done < "$file_path"


for i in "${!lines[@]}"; do
  echo "Line $((i+1)): ${lines[i]}"
done

lines_size=${#lines[@]}
if [[ "$lines_size" -ne "11" ]]; then
  echo "Not enough parameter."
  exit 1
fi


#checking
#Programming Language
output_str=$(echo "${lines[2]}" | sed 's/python/py/')
output_string=$(echo "$output_str" | sed 's/\r$//')
IFS=' ' read -r -a ext_array <<< "$output_string"
val_arr=("c" "cpp" "py" "sh")
for ext in $ext_array; do
  if ! printf '%s\n' "${val_arr[@]}" | grep -qx "$ext"; then
    echo "$ext"
    echo "Not intended File Format.Error in parameter 3"
    exit 1
  fi
done

#output File path->expected_output
expected_output=$(echo "${lines[7]}" | sed 's/\r$//')

#marks->total_matks
total_marks=$(echo "${lines[3]}" | sed 's/\r$//')
missing_marks=$(echo "${lines[4]}" | sed 's/\r$//')
if ! [[ "$total_marks" =~ ^-?[0-9]+$ ]]; then
  echo "Not intended File Format. Error in parameter 4"
  exit 1
fi

# missing_marks
if ! [[ "$missing_marks" =~ ^-?[0-9]+$ ]]; then
  echo "Missing marks is not an integer."
  echo "Not intended File Format.Error in parameter 5"
  exit 1
fi


#Roll->range 
output_string=$(echo "${lines[6]}" | sed 's/\r$//')
IFS=' ' read -r -a range <<< "$output_string"
array_size=${#range[@]}

if [[ "$array_size" -ne "2" ]]; then
  echo "Not intended File Format.Error in parameter 7"
  exit 1
fi


starting_roll="${range[0]}"
ending_roll="${range[1]}"
if ! [[ "$starting_roll" =~ ^-?[0-9]+$ ]]; then
  echo "Not intended File Format.Error in parameter 7"
  exit 1
fi

if ! [[ "$ending_roll" =~ ^-?[0-9]+$ ]]; then
  echo "Not intended File Format. Error in parameter 7"
  exit 1
fi


#Submission Violation->submission_violation
submission_violation=$(echo "${lines[8]}" | sed 's/\r$//')
if ! [[ "$submission_violation" =~ ^-?[0-9]+$ ]]; then
  echo "Not intended File Format. Error in parameter 9"
  exit 1
fi


#Working_Directory
dir=$(echo "${lines[5]}" | tr -d '\r')


#Archived->archived
archived=$(echo "${lines[0]}" | sed 's/\r$//')
echo "$archived"
val_arr=("true" "false")
  if ! printf '%s\n' "${val_arr[@]}" | grep -qx "$archived"; then
    echo "Not intended File Format. Error in parameter 1"
    exit 1
  fi


#Archived list->arc_list
output_string=$(echo "${lines[1]}" | sed 's/\r$//')
val_arr=("zip" "rar" "tar")
IFS=' ' read -r -a arc_list <<< "$output_string"
for ext in $arc_list; do
  if ! printf '%s\n' "${val_arr[@]}" | grep -qx "$ext"; then
    echo "Not intended File Format. Error in parameter 2"
    exit 1
  fi
done


  

#plagiarigm
pl_dir=$(echo "${lines[9]}" | tr -d '\r')
content=$(cat "$pl_dir")
IFS=$'\n'
pl_list=()  # Initialize an empty array
while read -r line; do
    pl_list+=("$line")  # Add each line to the array
done <<< "$content"
pl_marks=$(echo "${lines[10]}" | sed 's/\r$//')
if ! [[ "$pl_marks" =~ ^-?[0-9]+$ ]]; then
  echo "Not intended File Format. Error in parameter 11"
  exit 1
fi





#Associative arrays
declare -A A_marks
declare -A A_deducted_marks
declare -A A_total_marks
declare -A A_remarks
#normal arrays
issue_array=()
roll_array=()

#initializing arrays
for((i="$starting_roll";i<="$ending_roll";i++)); do
  A_marks["$i"]=0
  A_deducted_marks["$i"]=0
  A_total_marks["$i"]=0
done

size=$((ending_roll-starting_roll))

for((i=0;i<=size;i++)); do
 roll_array[i]=$((starting_roll+i))
done




#Functions

check_Language(){ 
        local file="$1"  
  if [ -f "$file" ]; then
    file_ext="${file##*.}"
      
    if printf '%s\n' "${ext_array[@]}" | grep -qx "$file_ext"; then
      echo "true"
    else
      echo "false"
    fi
  fi

}



run_file() {
    local file_path="$1"
    output_file="$2"

    if [[ -f "$file_path" ]]; then
        case "$file_path" in
            *.sh) 
                bash "$file_path" > "$output_file" ;;
            *.py) 
                python3 "$file_path" > "$output_file" ;;
                
            *.cpp)
                file_name=$(basename "$file_path")
                exec=$(echo "$file_name" | sed 's/\.[^.]*$//')
                g++ "$file_path" -o "$exec"
                ./"$exec" > "$output_file" ;;

                 *.c)
                file_name=$(basename "$file_path")
                exec=$(echo "$file_name" | sed 's/\.[^.]*$//')
                gcc "$file_path" -o "$exec"
                ./"$exec" > "$output_file" ;;
                *)
                echo "Unsupported file type: $file_path" ;;
        esac
    else
        echo "Error: File does not exist."
    fi
}


get_marks(){
missing_count=0
local_file="$1"
s_id="$2"
while IFS= read -r line; do
    
    if ! grep -Fxq "$line" "$local_file"; then
        ((missing_count++))
    fi
done < "$expected_output"
marks=$((total_marks - missing_marks * missing_count))
A_marks["$s_id"]=$marks
echo "Total marks: $marks"

}

create_clear_directory() {
    local dir=$1

    if [ -d "$dir" ]; then
        rm -rf "$dir"/*
    else
        mkdir -p "$dir"
    fi
}

create_unzipped(){
        file="$1"
        filename="$(basename "$file" | sed 's/\.[^.]*$//')"
        non_archived_dir="$dir/$filename"
        mkdir -p "$non_archived_dir"
        mv "$file" "$non_archived_dir"
        echo "$non_archived_dir"

}

#Directories
main_output="output"
mkdir -p "$main_output"
issues_dir="${main_output}/submissions/issues"
checked_dir="${main_output}/submissions/checked"
create_clear_directory "$issues_dir"
create_clear_directory "$checked_dir"
#CSV 
csv="${main_output}/submissions/marks.csv"
echo "id,marks,marks_deducted,total_marks,remarks" > "$csv"




# unzip all and move
for file in "$dir"/*; do    
    if [[ "$file" == *.zip ]]; then
      if ! printf '%s\n' "${arc_list[@]}" | grep -qx "zip"; then
        filename="$(basename "$file" | sed 's/\.[^.]*$//')"
        unzip "$file" -d "$issues_dir"
        A_remarks["$filename"]=" issue case#2"
        current_marks=${A_deducted_marks["$filename"]}
        A_deducted_marks["$filename"]=$((current_marks + submission_violation))

      else 
           if ! printf '%s\n' "${roll_array[@]}" | grep -qx "$filename"; then
            continue
            fi
        unzip "$file" -d "$checked_dir"
      fi

    elif [[ "$file" == *.rar ]]; then 
      if ! printf '%s\n' "${arc_list[@]}" | grep -qx "rar"; then
        filename="$(basename "$file" | sed 's/\.[^.]*$//')"
        unrar x "$file" "$issues_dir"
        A_remarks["$filename"]=" issue case#2"
        current_marks=${A_deducted_marks["$filename"]}
        A_deducted_marks["$filename"]=$((current_marks + submission_violation))
      else 
            if ! printf '%s\n' "${roll_array[@]}" | grep -qx "$filename"; then
            continue
            fi
        unrar x "$file" "$checked_dir"
      fi
      

    elif [[ "$file" == *.tar ]]; then  
          if ! printf '%s\n' "${arc_list[@]}" | grep -qx "tar"; then
            filename="$(basename "$file" | sed 's/\.[^.]*$//')"
            tar -xf "$file" -C "$issues_dir"
            A_remarks["$filename"]=" issue case#2"
            current_marks=${A_deducted_marks["$filename"]}
            A_deducted_marks["$filename"]=$((current_marks + submission_violation))
          else 
            if ! printf '%s\n' "${roll_array[@]}" | grep -qx "$filename"; then
            continue
            fi
            tar -xf "$file" -C "$checked_dir"
          fi

    elif [ -f "$file" ]; then
        filename="$(basename "$file" | sed 's/\.[^.]*$//')"
         if ! printf '%s\n' "${roll_array[@]}" | grep -qx "$filename"; then
         continue
         fi
        non_archived_dir="$dir/$filename"
        mkdir -p "$non_archived_dir"
        mv "$file" "$non_archived_dir/"
        if [[ "$archived" == "true" ]]; then
          A_remarks["$filename"]+=" issue case#1"
          current_marks=${A_deducted_marks["$filename"]}
          A_deducted_marks["$filename"]=$((current_marks + submission_violation))
          issue_array+=("$non_archive_dir")
        fi 
        mv "$non_archived_dir" "$checked_dir"

    elif [ -d "$file" ]; then 
         filename="$(basename "$file")"
         echo "$filename"
         if  printf '%s\n' "${roll_array[@]}" | grep -qx "$filename"; then
            if [[ "$archived" == "true" ]]; then
              A_remarks["$filename"]+=" issue case#1"
              current_marks=${A_deducted_marks["$filename"]}
              A_deducted_marks["$filename"]=$((current_marks + submission_violation))
              issue_array+=("$non_archive_dir")
            fi
        mv "$file" "$checked_dir"
         fi
        
    fi
done



#Iterate all directory
traverse="$starting_roll"

for folder in "$checked_dir"/*; do

  if [ -d "$folder" ]; then
    echo "Directory found: $(basename "$folder")"
    student_id="$(basename "$folder")"

    while [[ "$traverse" -ne "$student_id" ]]; do
      A_remarks["$traverse"]+=" missing submission"
      traverse=$((traverse + 1))
      continue
    done

      if [[  "$traverse" -gt "$ending_roll" ]]; then
        break
      fi
  
        file=$(find "$folder" -maxdepth 1 -type f | head -n 1)  
        output=$(check_Language "$file")

        if [[ "$output" == "true" ]]; then
                file_id="$(basename "$file")"
                base_name="${file_id%%.*}"
                output_file="$folder/${base_name}_output.txt"

                if [[ "$base_name" != "$student_id" ]]; then
                  current_marks=${A_deducted_marks["$student_id"]}
                  A_deducted_marks["$student_id"]=$((current_marks + submission_violation))
                  A_remarks["$student_id"]+=" issue case#4"
                fi

                if [ -x "$file" ]; then
                        dos2unix "$file" 
                        run_file "$file" "$output_file"
                        get_marks "$output_file" "$student_id"
                else
                        echo "Program not found"
                 fi

    elif [[ "$output" == "false" ]]; then
        A_remarks["$student_id"]+=" issue case#3"
        current_marks=${A_deducted_marks["$student_id"]}
        A_deducted_marks["$student_id"]=$((current_marks + submission_violation))
        mv "$folder" "$issues_dir"
    fi
  fi
   traverse=$((traverse+1))
done




#csv write


for roll in "${!A_marks[@]}"; do
    A_total_marks["$roll"]=$((A_marks[$roll] - A_deducted_marks[$roll]))
done

for((i="$starting_roll";i<="$ending_roll";i++)); do
  if  printf '%s\n' "${pl_list[@]}" | grep -qx "$i"; then
    A_remarks["$i"]+=" plagiarism detected"
    A_total_marks["$i"]="-${pl_marks}"
  fi
done
sorted_marks=($(for key in "${!A_marks[@]}"; do echo "$key"; done | sort))
sorted_deducted_marks=($(for key in "${!A_deducted_marks[@]}"; do echo "$key"; done | sort))
sorted_total_marks=($(for key in "${!A_total_marks[@]}"; do echo "$key"; done | sort))
sorted_remarks=($(for key in "${!A_remarks[@]}"; do echo "$key"; done | sort))


for roll in "${sorted_marks[@]}"; do
    echo "$roll,${A_marks[$roll]}","${A_deducted_marks[$roll]}","${A_total_marks[$roll]}","${A_remarks[$roll]}" >> "$csv"
done

for folder in "$checked_dir"/*; do 
  if [ -d "$folder" ]; then
    student_id="$(basename "$folder")"
    if  printf '%s\n' "${issue_array[@]}" | grep -qx "$student_id"; then
    mv "$folder" "$issue_dir"
    fi
  fi
done







