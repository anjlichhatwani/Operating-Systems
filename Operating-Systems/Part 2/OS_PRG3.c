#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

/*Each student has an id, the number of questions he/she wants to ask, and the question number that the student is on. */
struct student{
	int id;
	int numQuestions;
	int questionNum;
	struct student *next;
	struct student *previous;
};

unsigned int condition, condition2;
unsigned int answeredQs;
unsigned int numStud;
unsigned int roomSize;
unsigned int professorLeave = 0;//A value other than 0 signifies that the professor has finished all the 
pthread_cond_t student, prof;
pthread_mutex_t lock, lock_t, lock_t2, lock_t3;
pthread_t P;
struct student *iterator = NULL, *head = NULL;

void * startProfessor();
void * startStudent(void * s);
void AnswerStart();
void AnswerDone();
void QuestionStart();
void QuestionDone();
void professorIsLeaving();
bool isPositiveNumber(char number[]);
void enterOffice(struct student* stud);
void leaveOffice();

/* This function starts the Professor thread that runs a loop calling AnswerStart and 
AnswerDone. If no students are around, he naps.*/
void Professor(){
	// initialize the mutex
	pthread_mutex_init(&lock, NULL);
	pthread_mutex_init(&lock_t, NULL);
	pthread_mutex_init(&lock_t2, NULL);
	pthread_mutex_init(&lock_t3, NULL);
	
	// initialize the condition variables
	pthread_cond_init(&prof, NULL);
	pthread_cond_init(&student, NULL);	
	
	// initialize the global variables
	numStud = 0; 	//keeps track of the number of students
	condition = 0; //wakes up professor
	condition2 = 1;//wakes up student
	
	if (pthread_create(&P, NULL, startProfessor,NULL) != 0){
		perror("Thread creation failed");
		exit(0);
	}
	sleep(1);
}

/* The professor thread  */
void* startProfessor(){
	puts("\nProfessor's office hours have begun!\n");
	condition = 1;
	while(1){
		
		if( numStud == 0 ){ professorIsLeaving(); } 
		
		pthread_cond_wait(&prof, &lock_t); // wait until a student asks a question
		
		condition = 0;
		if(head != NULL){ 
			AnswerStart(); 
			AnswerDone();
			
			// signal student that the question has been answered		
			while(condition2){ pthread_cond_signal(&student); }
			condition2 = 1;
		}
	}
}

/*Creates a thread that represents a new student with identifier id that asks the professor a specific number of questions. ID can be expected to be >= 0.*/
void Student(int id){
	struct student * newstud =  malloc(sizeof(struct student));
	
	newstud->id = id;
	newstud->numQuestions = (id % 4) + 1;
	newstud->questionNum = 0;
	
	pthread_t S; // local stack variable
	
	//create thread
	if(pthread_create( &S, NULL, (void *) &startStudent, (void *)newstud ) != 0){
		perror("Thread creation failed");
		exit(0);
	}
}

/* The student thread */
void * startStudent(void * s){	
	struct student * stud = s;
	
	enterOffice(stud);
	
	//each student loops running the code for the number of questions he asks:	
	while(1){
		pthread_mutex_lock(&lock);

		if(head != NULL && numStud == 0){
			head = NULL;
			pthread_cond_signal(&prof);
		}
		
		if(head != NULL){ // if there are students in the queue
			QuestionStart();
			QuestionDone();
			head = head->next;
		}
		pthread_mutex_unlock(&lock);
	}
}

// The method checks if the room is full or not and only allows a person to enter if there is space in the room.
void enterOffice(struct student* stud){
	int count =0;
	pthread_mutex_lock(&lock_t3);
	if(head != NULL){ 
		
		while(1){
			if(numStud < roomSize){
				printf("\nStudent %i enters Professor's room and wants to ask %i questions.\n", stud->id, stud->numQuestions);
				stud->previous = head;
				stud->next = head->next;
				head->next->previous = stud;
				head->next = stud;
				
				numStud++;// increment the number of students
				break;
			} else {
				if(count == 0){
					printf("\nstudent %i is waiting at the door\n",stud->id);
					count = 1;
				}
			}
		}
	} // add student to list
	else{ 
		condition = 1; 
		head = stud;
		head->next = head;
		head->previous = head;
		printf("\nStudent %i enters Professor's room and wants to ask %i questions.\n", stud->id, stud->numQuestions);
		numStud++;// increment the number of students
	}
	pthread_mutex_unlock(&lock_t3);
}

/*It is the turn of the student to ask his next question (his nth question). Wait to print out the message until it is really that student's turn.*/
void QuestionStart(){	
	(head->questionNum)++; // increment the question number
	(head->numQuestions)--; // decrement the number of questions left
	
	printf("\nStudent %i asks question # %i.\n", head->id, head->questionNum);
	
	while(condition){ pthread_cond_signal(&prof); }
	condition = 1;
	
	pthread_cond_wait(&student, &lock_t2);
	condition2 = 0;
}

/*The student is satisfied with the answer. Since professors consider it rude for a student not to wait for an answer, QuestionDone() should not print anything until the professor has finished answering the question.*/
void QuestionDone(){
	printf("\nStudent %i is satisfied.\n", head->id);
	
	if(head->numQuestions == 0){ // if the number of questions is zero, remove the student
		leaveOffice();
	}
}

void leaveOffice(){
	--numStud;
	printf("\nStudent %i is leaving the office\n", head->id);
	head->next->previous = head->previous;
	head->previous->next = head->next;
	free(head);
	head = head->next;
	// no more students
	if(head == NULL){ pthread_cond_signal(&prof); }
}

/*The professor starts to answer the next (nth) question of student X.*/
void AnswerStart(){
	printf("\nProfessor starts to answer question # %i for student %i.\n", head->questionNum, head->id);
}

/*The professor is done answering a question of a student. This should be done after a finite amount of time which you can simulate using a busy-wait call counting up to random number.*/
void AnswerDone(){	
	int rand = (random() % 1000) + 1;
	printf("\nProfessor is done with answering question # %i for student %i.\n",head->questionNum, head->id); 
	usleep(rand);
}

// The method checks if the professor is done for the day or not.
void professorIsLeaving(){ 
	if(professorLeave == 0){
		professorLeave++;
	} else {
		printf("\nProfessor's office hours have finished. Professor is leaving the office\n\n");
		exit(0);
	}
}

// Utility method to check if input character array represents a valid positive number or not.
bool isPositiveNumber(char number[])
{
    int i = 0;

    //checking for negative numbers
    if (number[0] == '-'){
		return false;
	}
    for (; number[i] != 0; i++)
    {
        //if (number[i] > '9' || number[i] < '0')
        if (!isdigit(number[i]))
            return false;
    }
    return true;
} 

// The main method for the file.
int main(int argc, char *argv[]){
	bool isNum = false;
	int numberStudent;
	int i =0;
	char *endptr;
	
	if( argc != 3 )
    	{
        	printf("Invalid arguments /n");
        	return 0;
    	}
		
	isNum = isPositiveNumber(argv[1]);
	if(isNum){
	   printf("\nThe input argument value for number of students is a positive number\n");
	} else {
		printf("\nThe input argument value for number of students is not a positive number\n");
		return EXIT_SUCCESS;
	}
	
	isNum = isPositiveNumber(argv[2]);
	if(isNum){
	   printf("\nThe input argument value for room size is a positive number\n");
	} else {
		printf("\nThe input argument value for room size is not a positive number\n");
		return EXIT_SUCCESS;
	}
   
	numberStudent = strtol(argv[1], &endptr, 10);
	roomSize = strtol(argv[2], &endptr, 10);
	
	Professor();
	i=0;
	
	for(i=0;i<numberStudent;i++) {
		Student(i);       
	} 
	while (1);

	return 0;
}

