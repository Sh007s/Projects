<<Documentation
NAME         : Shankar s
DATE         : 06/10/2022
DESCRIPTION  : Command line test 
SAMPLE INPUT : 
SAMPLE OUTPUT: 


<------------------------------------------------------------RESULT---------------------------------------------------------->


1. Unit of thermal conductivity in M.K.S units is _______________
a) kcal/kg m2°C
b) kcal-m/hr m2°C
c) kcal/hr m2°C
d) kcal-m/hr°C
e) kcal-m/m2°C
You Entered answer is: c
Correct answer is: b
mark = 0
Worng Answer

2. Unit of thermal conductivity in S.I units is _______________
a) j/m2 sec
b) J/m °K sec
c) W/m °K
d) a. and c. above
e) b. and c. above
You Entered answer is: a
Correct answer is: e
mark = 0
Worng Answer

3. Thermal conductivity of solid metals with rise in temperature normally
a) increases
b) decreases
c) remains constant
d) may increases or decrease depending on temperature
e) unpredictable.
You Entered answer is: b
Correct answer is: b
mark = 1
Rigth Answer

4. Heat transfer takes place as per--
a) zeroth law of thermodynamics
b) first law of thermodynamics
c) second law of thermodynamics
d) Kirchoff's law E.Stefan's law
e) none of the above
You Entered answer is: c
Correct answer is: c
mark = 1
Rigth Answer

5. Heat is closely related with
a) liquids
b) energy
c) temperature
d) entropy
e) enthalpy
You Entered answer is: c
Correct answer is: c
mark = 1
Rigth Answer

Your Total Score is 3

Documentation

#!/bin/bash

echo -e "Please choose the below options:\n"                                 # print the choose option
echo -e "1. Sign up\n2. Sign in\n3. Exit"                                    # print the 1.option sign up 2.sign in 3.exit 
read -p "Please choose your option: " op                                     # read the choose option
username=(`cat username.csv|tr "," " "`)                                     # initialize username and display the username
password=(`cat password.csv|tr "," " "`)                                     # initialize password and display the password
length=`expr ${#username[@]} - 1`                                            # initialize length and generate the loop in user name
case $op in				                                       # initialize case statement
    1)
	count=0				                                       #intialize the count
	while [ $count -eq 0 ]		                                                #using while condition compare count
	do
	    flag=0			                                               #initialize the flag=0
	    read -p "Enter the user name: " user                                     # read the user name
	    if [ ${#username[@]} -eq 0 ]					        # using if condtion comapare the user name 
	    then
	        read -p "Enter the password: " passwd					# read the password 
		characters=$(echo -n "$passwd" |wc -w | echo -n "$passwd" |wc -c)	# count the characters using the word count
	        if [ $characters -gt 8 ]						#comapare the characters equal to 8 and above or not
	       	then
	       	    read -p "Conform the password: " cpasswd		        #read the confirm password
	            if [ $passwd = $cpasswd ]						#comapare password and confirm password
	            then
	                echo $passwd >> password.csv					#using the echo and redirection move password.csv 
		        echo $user >> username.csv					#using the echo and redirection move username.csv
			echo "created the account successfully"			#print the statement after signup account
		        count=1					                #count = 1
	            else		
	                echo "This is Invalid Passward"				#print else statement if condition false
	            fi
	        else
	            echo "please enter the 8 characters"    				#print else statement  if below 8 characters
	        fi
	    else									
	        for name in ${username[@]}						#using for loop enter the name 
	        do
	            if [ $name = $user ]					        #compare the user name same as entered name 
	            then
		        echo "This is Invalid Username"                               #if not match print invalid username
			flag=1                                                        #set the flag=1
	            fi
	        done
		if [ $flag -eq 0 ]							#compare the flag is equal to zero
		then
	            read -p "Enter the password: " passwd				# read the entered password
	            read -p "Confirm the password: " cpasswd				#read the confirm password
	            if [ $passwd = $cpasswd ]					        # check the entered password is matching or not
	            then
	                echo $passwd >> password.csv					#using echo and redirection print and move the contents
		        echo $user >> username.csv					#using echo and redirection print and move the contents
			echo "created the account successfully"			#using echo command print the statement
		        count=1				              		#set the count=1
	            else
	                echo "This is Invalid Password"				#print else statement if statement is wrong
		    fi
		fi
	    fi
	done
	;;										#break the case statement
    2)
	flag=0										#set the flag=0
	count=3									#set the count=3
	total=0									#initialize the count=0
	while [ $flag -eq 0 ]								# using while loop flag is 0 or not
	do
	    read -p "Please Enter the user name: " user				#read the user name 
	    for index in `seq 0 $length `						#using for loop seq 0 to length
	    do
	        if [ $user = ${username[$index]} ]					#using if statement user and user name index
	        then
		    while [ $count -ne 0 ]					        #using the while compare the count is not equal 0
		    do
			echo "Please Enter the Password: "				#using echo command print the statement
		        read -s passwd							#read the passwd is file or not
		        if [ $passwd = ${password[$index]} ]				#using if statement comapare entered password same as original password
		        then
			    clear							#clear the previous data
		            echo -e "Login Sucessful\n"				#using echo command print the login successful 
			    echo -e "1. Take Test\n2. Exit"				#using echo command print the options
			    read -p "choose option below: " opp			#using echo command print the choose options
			    case $opp in						#initialize the case statement
				1)							
				    clear
				    echo "ALL THE BEST FOR THE EXAM"			#print the statement using echo command
				    for i in `seq 6 6 30`				#using the for loop generate sequence
				    do
                                        head -$i questions.txt|tail -6               #using head command print the csv file text
                                        for sec in `seq 10 -1 1` 			#using the for loop generate sequence
                                        do
	                                    echo -e "\r Enter the Option: $sec \c"	#print the statement using echo command
	                                    read -t 1 choice				#read the choice
					    if [ -n "$choice" ]			#using if condition compare the choice
					    then
					        echo $choice >> userans.txt            #using echo and redirection print and move the contents
						break					#break
					    fi
                                        done
					if [ -n "$choice" ]				#using the if condition check the choices
					then
					    continue                                   #continue the condition
					else
					    choice='e'                                 #choice e if wrong ans entered
					    echo $choice >> userans.txt                #using echo and redirection print and move the contents
					fi
					echo
				    done
				    userans=(`cat userans.txt`)			# initialize userans and display the user ans
				    correctans=(`cat correctans.txt`)			# initialize correct ans and display the correct ans
				    clear
				    echo -e "\n<------------------------------------------------------------RESULT---------------------------------------------------------->\n\n"
				    index=0						    #initialize the index 0
				    for i in `seq 6 6 30`				    #using the for loop generate seq
				    do
					head -$i questions.txt|tail -6                              #using head command print the csv file text
					echo "You Entered answer is: ${userans[$index]}"            #using echo command print the user answer index 
					echo "Correct answer is: ${correctans[$index]}"             #using echo command print the correct answer index
					if [ ${userans[$index]} = ${correctans[$index]} ]           #using if statement comapre entered answer same as original correct ans
					then
					    total=`expr $total + 1`                                 #intialize the total and count
					    echo -n -e "mark = 1\n"			              #using echo command print mark =1 if correct ans
					         if [ $mark =1 ]                                           #using if statement print rigth ans
					         then
					             echo -e "\e[32mRigth Answer\e[0m"                       #green color
					         fi			                       
					 else
					    echo -n -e "mark = 0\n"		                      #using echo command print mark =0 if wrong ans	
					         if [ $mark =0 ]                                            #using if statment print worng ans
					         then
					             echo -e "\e[31mWorng Answer\e[0m"                      #red colors
					         fi		
					fi
				    	index=`expr $index + 1`        				#initialize index and count the c
				    	done
				    echo -e "\e[33m\nYour Total Score is $total\e[0m"		         #using echo and redirection print and move the contents
				    echo  "" > userans.txt                                     #using echo and redirection print and move the contents
				    ;;
				2)
				    exit
			    esac	
			    count=0							          #intialize the count is 0
		        else 
			    echo "You Entered Invalid Password please try again"	          #using echo command print the statement
			    count=$(($count - 1))					           #initialize the count 
		        fi
		    done
		    flag=1			                                                  # set flag is 0
	        fi
	    done
	    if [ $flag -eq 0 ]			                                                      # if statement compare flag is equal 0
	    then
		echo "This Username not Found!!"                                               #print the statement if flag is equal 0
	    fi
	done
	;;                                                                                         # break the case statement
    3)
	exit
esac                                                                                                  #end case statement
