#include<chrono>
#include<fstream>
#include<iostream>
#include<pthread.h>
#include<random>
#include<unistd.h>
#include<vector>
#include<semaphore.h>
#include<algorithm>

using namespace std;

#define SLEEP_MULTIPLIER 1000

int N,M; //N-># of standard M-># of premium
int w,x,y,z; //w->spent in hallway,x->spent in gallery1, y->spent in gallery2, z->spent on photobooth
auto start_time=chrono::high_resolution_clock::now();
pthread_mutex_t step_0_lock;
pthread_mutex_t step_1_lock;
pthread_mutex_t step_2_lock;
pthread_mutex_t print;
pthread_mutex_t waiting;
pthread_mutex_t db;
pthread_mutex_t std_lock;
pthread_mutex_t prm_lock;

sem_t gallery_1;
sem_t corridor;

int premium_count=0;
int standard_count=0;



class visitor{
   public:
        int id;
        int delay;
        int a=0;
        int b=0;
        int s1=0;
        int s2=0;
        int s3=0;
        int c=0;
        int d=0;
        int e=0;
        int wait=0;
        int pb=0;
        int exit=0;

        visitor(int uid,int udelay){
                id=uid;
                delay=udelay;
        }

};

int get_random_number() {
  random_device rd;

  mt19937 generator(rd());
  double lambda = 5.234;
  poisson_distribution<int> poissonDist(lambda);
  return poissonDist(generator);
}
long long get_time() {
  auto end_time = chrono::high_resolution_clock::now();
  auto duration = chrono::duration_cast<chrono::milliseconds>(
      end_time - start_time);
  long long elapsed_time_ms = duration.count();
  return elapsed_time_ms;
}

void write_output(string output) {
  pthread_mutex_lock(&print);
  cout << output;
  pthread_mutex_unlock(&print);
}

bool compareByA(const visitor &X, const visitor &Y) {
    return X.a < Y.a;
}

bool compareByB(const visitor &X, const visitor &Y){
        return X.b<Y.b;
}

bool compareByS1(const visitor &X, const visitor &Y){
        return X.s1<Y.s1;
}

bool compareByC(const visitor &X, const visitor &Y){
        return X.c<Y.c;
}

bool compareByD(const visitor &X, const visitor &Y){
        return X.d<Y.d;
}
bool compareByE(const visitor &X, const visitor &Y){
        return X.e<Y.e;
}

bool compareByWait(const visitor &X, const visitor &Y){
        return X.wait<Y.wait;
}

bool compareByPB(const visitor &X, const visitor &Y){
        return X.pb<Y.pb;
}

bool compareByExit(const visitor &X, const visitor &Y){
        return X.exit<Y.exit;
}



void * visitor_activity(void* arg){
        visitor *curr_visitor=(visitor*)arg;
        int id=curr_visitor->id;
        int delay=curr_visitor->delay;

        //Before entering hallway
        usleep(delay*SLEEP_MULTIPLIER);
        curr_visitor->a=get_time();
        //write_output("Visitor "+to_string(id)+" has arrived at A at timestamp "+to_string(get_time())+"\n");
        //Entered the hallway
        usleep(w*SLEEP_MULTIPLIER);
        curr_visitor->b=get_time();
        //write_output("Visitor "+to_string(id)+" has arrived at B at timestamp "+to_string(get_time())+"\n");
        

        //In the steps
        usleep(1* SLEEP_MULTIPLIER);
        pthread_mutex_lock(&step_0_lock);
        curr_visitor->s1=get_time();
        //write_output("Visitor "+to_string(id)+" has arrived at step 1 at timestamp "+to_string(get_time())+"\n");
        usleep(1* SLEEP_MULTIPLIER);

        pthread_mutex_lock(&step_1_lock);
        pthread_mutex_unlock(&step_0_lock);
        curr_visitor->s2=get_time();
        //write_output("Visitor "+to_string(id)+" has arrived at step 2 at timestamp "+to_string(get_time())+"\n");
        usleep(1* SLEEP_MULTIPLIER);
        
        pthread_mutex_lock(&step_2_lock);
        pthread_mutex_unlock(&step_1_lock);
        curr_visitor->s3=get_time();
        //write_output("Visitor "+to_string(id)+" has arrived at step 3 at timestamp "+to_string(get_time())+"\n");
        usleep(1* SLEEP_MULTIPLIER);


        
        //Exits step ; Enter into the gallery-1
        sem_wait(&gallery_1);
        pthread_mutex_unlock(&step_2_lock);
        curr_visitor->c=get_time();
        //write_output("Visitor "+to_string(id)+" has arrived at C (entered Gallery 1) at timestamp "+to_string(get_time())+"\n");
        
        usleep(x*SLEEP_MULTIPLIER);
        sem_wait(&corridor);
        sem_post(&gallery_1);
        curr_visitor->d=get_time();
        //write_output("Visitor "+to_string(id)+" has arrived at D (exiting Gallery 1) at timestamp "+to_string(get_time())+"\n");
        usleep(delay*SLEEP_MULTIPLIER);

        sem_post(&corridor);
        curr_visitor->e=get_time();
        //write_output("Visitor "+to_string(id)+" has arrived at E (Entered Gallery 2) at timestamp "+to_string(get_time())+"\n");
        usleep(y*SLEEP_MULTIPLIER);
        curr_visitor->wait=get_time();
        //write_output("Visitor "+to_string(id)+" is about to enter the photobooth at timestamp "+to_string(get_time())+"\n");
        //usleep(1*SLEEP_MULTIPLIER);

        if(id>=2000 && id<=2100){
                //Previleged->immediate and exclusive
                pthread_mutex_lock(&prm_lock);
                premium_count++;
                if(premium_count==1){pthread_mutex_lock(&waiting);}
                pthread_mutex_unlock(&prm_lock);
                

                pthread_mutex_lock(&db);
                 curr_visitor->pb=get_time();
                //write_output("Visitor "+to_string(id)+" is inside the photobooth at timestamp "+to_string(get_time())+"\n");
                usleep(z*SLEEP_MULTIPLIER);
                pthread_mutex_unlock(&db);
                 curr_visitor->exit=get_time();
                //write_output("Visitor "+to_string(id)+" exited the gallery-2 at timestamp "+to_string(get_time())+"\n");

                pthread_mutex_lock(&prm_lock);
                premium_count--;
                if(premium_count==0){pthread_mutex_unlock(&waiting);}
                pthread_mutex_unlock(&prm_lock);
                
        }
        else{
                pthread_mutex_lock(&waiting);
                pthread_mutex_lock(&std_lock);
                standard_count++;
                if(standard_count==1){pthread_mutex_lock(&db);}
                pthread_mutex_unlock(&std_lock);
                pthread_mutex_unlock(&waiting);
                 curr_visitor->pb=get_time();
                //write_output("Visitor "+to_string(id)+" is inside the photobooth at timestamp "+to_string(get_time())+"\n");
                 usleep(z*SLEEP_MULTIPLIER);
                  curr_visitor->exit=get_time();
                //write_output("Visitor "+to_string(id)+" exited the gallery-2 at timestamp "+to_string(get_time())+"\n");
                pthread_mutex_lock(&std_lock);
                standard_count--;
                if(standard_count==0){pthread_mutex_unlock(&db);}
                pthread_mutex_unlock(&std_lock);


        }



        return NULL;

}





int main(int argc,char* argv[]){

        if(argc!=7){
                cerr<<"Please provide 6 integer values"<<endl;
                return 1;
        }
        vector<visitor> visitors;
        int total;
        N=atoi(argv[1]);
        M=atoi(argv[2]);
        w=atoi(argv[3]);
        x=atoi(argv[4]);
        y=atoi(argv[5]);
        z=atoi(argv[6]);

        /*cout<<"Enter the value of N and M"<<endl;
        cin>>N>>M;
        cout<<endl;
        cout<<"Enter the value of w,x,y,z"<<endl;
        cin>>w>>x>>y>>z;
        cout<<endl;*/

        total=N+M;

        for(int i=0;i<N;i++){
                int rnd=get_random_number();
                visitor temp(1001+i,rnd);
                visitors.push_back(temp);

        }

        for(int i=0;i<M;i++){
                int rnd=get_random_number();
                visitor temp(2001+i,rnd);
                visitors.push_back(temp);

        }

       /*for(int i=0;i<N+M;i++){
                cout<<visitors[i].id<<" "<<visitors[i].delay<<endl;
        }*/
        pthread_mutex_init(&step_0_lock,NULL);
        pthread_mutex_init(&step_1_lock,NULL);
        pthread_mutex_init(&step_2_lock,NULL);
        pthread_mutex_init(&print,NULL);
        pthread_mutex_init(&db,NULL);
        pthread_mutex_init(&waiting,NULL);
        pthread_mutex_init(&std_lock,NULL);
        pthread_mutex_init(&prm_lock,NULL);
        sem_init(&gallery_1,0,5);
        sem_init(&corridor,0,3);
        start_time = chrono::high_resolution_clock::now(); 

       //Threading
       pthread_t visitor_thread[total];
       
       for(int i=0;i<total;i++){
        pthread_create(&visitor_thread[i],NULL,visitor_activity,&visitors[i]);
       }

         for (int i = 0; i < total; i++) {
        pthread_join(visitor_thread[i], NULL);
        }

        cout<<"Printing the time"<<endl;

        sort(visitors.begin(), visitors.end(), compareByA);
        for(int i=0;i<total;i++){
                write_output("Visitor "+to_string(visitors[i].id)+" has arrived at A at timestamp "+to_string(visitors[i].a)+"\n");
        }

        cout<<"..................................."<<endl;
        cout<<"A visitor spends "<<w<<" unit of time at Hallway"<<endl;
        cout<<"..................................."<<endl;

        sort(visitors.begin(), visitors.end(), compareByB);
        for(int i=0;i<total;i++){
                write_output("Visitor "+to_string(visitors[i].id)+" has arrived at B at timestamp "+to_string(visitors[i].b)+"\n");
        }

        cout<<"..................................."<<endl;

        sort(visitors.begin(), visitors.end(), compareByS1);
        for(int i=0;i<total;i++){
                write_output("Visitor "+to_string(visitors[i].id)+" has arrived at step 1 at timestamp "+to_string(visitors[i].s1)+"\n");
                write_output("Visitor "+to_string(visitors[i].id)+" has arrived at step 2 at timestamp "+to_string(visitors[i].s2)+"\n");
                write_output("Visitor "+to_string(visitors[i].id)+" has arrived at step 3 at timestamp "+to_string(visitors[i].s3)+"\n");
                
        }

        cout<<"..................................."<<endl;

        sort(visitors.begin(), visitors.end(), compareByC);
        for(int i=0;i<total;i++){
                write_output("Visitor "+to_string(visitors[i].id)+" has arrived at C (Entered gallery-1) at timestamp "+to_string(visitors[i].c)+"\n");
        }

        cout<<"..................................."<<endl;
        cout<<"A visitor spends "<<x<<" unit of time at Gallery 1"<<endl;
        cout<<"..................................."<<endl;

        sort(visitors.begin(), visitors.end(), compareByD);
        for(int i=0;i<total;i++){
                write_output("Visitor "+to_string(visitors[i].id)+" has arrived at D (Exiting gallery-1) at timestamp "+to_string(visitors[i].d)+"\n");
        }

        cout<<"..................................."<<endl;
        cout<<"A visitor spends arbitray time inside the glass corridor"<<endl;
        cout<<"..................................."<<endl;

        sort(visitors.begin(), visitors.end(), compareByE);
        for(int i=0;i<total;i++){
                write_output("Visitor "+to_string(visitors[i].id)+" has arrived at E (Entered gallery-2) at timestamp "+to_string(visitors[i].e)+"\n");
        }

        cout<<"..................................."<<endl;
         cout<<"A visitor spends "<<y<<" unit of time at Gallery 2 before reaching the photobooth"<<endl;
        cout<<"..................................."<<endl;
        sort(visitors.begin(), visitors.end(), compareByWait);
        for(int i=0;i<total;i++){
                write_output("Visitor "+to_string(visitors[i].id)+" is about to enter photobooth at timestamp "+to_string(visitors[i].wait)+"\n");
        }

        cout<<"..................................."<<endl;
        sort(visitors.begin(), visitors.end(), compareByPB);
        for(int i=0;i<total;i++){
                write_output("Visitor "+to_string(visitors[i].id)+" is inside photobooth at timestamp "+to_string(visitors[i].pb)+"\n");
        }

        cout<<"..................................."<<endl;
        cout<<"A visitor spends "<<z<<" unit of time at the photobooth"<<endl;
        cout<<"..................................."<<endl;

        sort(visitors.begin(), visitors.end(), compareByExit);
        for(int i=0;i<total;i++){
                write_output("Visitor "+to_string(visitors[i].id)+" exited gallery-2 at timestamp "+to_string(visitors[i].exit)+"\n");
        }

        cout<<"..................end.................."<<endl;


        //test
        /*cout<<"For the initial-1 :"<<endl;
        cout<<"a : "<<visitors[0].a<<" b: "<<visitors[0].b<<" s1: "<<visitors[0].s1<<" s2: "<<visitors[0].s2<<" s3: "<<visitors[0].s3<<endl;
        cout<<"c : "<<visitors[0].c<<" d: "<<visitors[0].d<<" e: "<<visitors[0].e<<" wait: "<<visitors[0].wait<<" pb: "<<visitors[0].pb<<" exit: "<<visitors[0].exit<<endl;*/

        return 0;



}