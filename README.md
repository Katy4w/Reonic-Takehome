# Reonic-Takehome

I finished all of task 1 but not all of task 2. Both tasks should run without any errors, here are instructions on how to run them (as long as you are on mac or linux).

To run task 1: type the following into your console:
```rb
cd Task_1
make
./a.out
```

if you're not using gcc you might need to change "gcc" to your prefered c compiler in the make file.

To run task 2, you might want to create a python virtual environment in the Task 2 folder:
```rb
cd Task_2
pwd
python3 -m venv /path/to/current/directory/.venv
source .venv/bin/activate
pip install -r requirements.txt
flask run
```
Then you can go to http://127.0.0.1:5000 to view the application.

Notes: 
task 1 only prints to the console, and inputs are predefined in the inputs.h file. 
I did not link task 1 and task 2.
