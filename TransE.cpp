string version;
char buf[100000],buf1[100000];
int relation_num,entity_num;
map<string,int> relation2id,entity2id;
map<int,string> id2entity,id2relation;


//all these dictionaries have been explained as and when required
map<int,map<int,int> > left_entity,right_entity;
map<int,double> left_num,right_num;

class Train{

public:
    map<pair<int,int>, map<int,int> > ok;
    void add(int x,int y,int z)
    {
    	//this dictionary is used for keeping the freebase entities left side.
        fb_h.push_back(x);
        //this is used for keeping the relationship label
        fb_r.push_back(z);
        //this dictionary is used for keeping the freebase entities right side.
        fb_l.push_back(y);
        //this is used for keeping the relationship like this (x is related to y by the relation z)
        ok[make_pair(x,z)][y]=1;
    }
    void run(int n_in,double rate_in,double margin_in,int method_in)
    {
    	//
        n = n_in;
        rate = rate_in;
        margin = margin_in;
        method = method_in;
        //represents an array of n dimensional relationship vectors
        relation_vec.resize(relation_num);
        	// and allocate each individual vector a size of n(dimension of embedding)
		for (int i=0; i<relation_vec.size(); i++)
			relation_vec[i].resize(n);
	//represents an array of n dimensional entity vectors
        entity_vec.resize(entity_num);
        	// and allocate each individual vector a size of n
		for (int i=0; i<entity_vec.size(); i++)
			entity_vec[i].resize(n);
	//temporary array of n dimensional relation vectors
        relation_tmp.resize(relation_num);
        	// and allocate each individual vector a size of n(dimension of embedding)
		for (int i=0; i<relation_tmp.size(); i++)
			relation_tmp[i].resize(n);
	//temporary array of n dimensional entity vectors
        entity_tmp.resize(entity_num);
        	// and allocate each individual vector a size of n(dimension of embedding)
		for (int i=0; i<entity_tmp.size(); i++)
			entity_tmp[i].resize(n);
	//iterating through all relationship ids
        for (int i=0; i<relation_num; i++)
        {
            //this is initializing the relationship vector with initial random values.
            //relation_vec[i] is the relationship vector corresponding to the relationshipid 'i'
            for (int ii=0; ii<n; ii++)
                relation_vec[i][ii] = randn(0,1.0/n,-6/sqrt(n),6/sqrt(n));
        }
        //iterating through all entity ids
        for (int i=0; i<entity_num; i++)
        {
            //this is initializing the entity vector with initial random values.
            //entity_vec[i] is the entity vector corresponding to the entityid 'i'
            for (int ii=0; ii<n; ii++)
                entity_vec[i][ii] = randn(0,1.0/n,-6/sqrt(n),6/sqrt(n));
            // this is normalising the entity vector by divinding each entry in the vector by the size of the vector
            norm(entity_vec[i]);
        }


        bfgs();
    }

private:
    int n,method;
    double res;//loss function value
    double count,count1;//loss function gradient
    double rate,margin;
    double belta;
    vector<int> fb_h,fb_l,fb_r;
    vector<vector<int> > feature;
    vector<vector<double> > relation_vec,entity_vec;
    vector<vector<double> > relation_tmp,entity_tmp;
    double norm(vector<double> &a)
    {
        double x = vec_len(a);
        if (x>1)
        for (int ii=0; ii<a.size(); ii++)
                a[ii]/=x;
        return 0;
    }
    int rand_max(int x)
    {
        int res = (rand()*rand())%x;
        while (res<0)
            res+=x;
        return res;
    }

    void bfgs()
    {
        res=0;
        int nbatches=100; //number of batches
        int nepoch = 1000; // number of iterat
        //fb_h has all the head entities in the dataset
        int batchsize = fb_h.size()/nbatches; // number of elements to be used in a batch
            //main for loop used for each individual iteration
            for (int epoch=0; epoch<nepoch; epoch++)
            {

            	res=0;
            	//iterating through the number of batches
             	for (int batch = 0; batch<nbatches; batch++)
             	{	
             		//working on the current relationship vector by storing it in the temporary vector
             		relation_tmp=relation_vec;
            		entity_tmp = entity_vec;
            		//iterating through the batches
             		for (int k=0; k<batchsize; k++)
             		{
						int i=rand_max(fb_h.size());
						int j=rand_max(entity_num);
						double pr = 1000*right_num[fb_r[i]]/(right_num[fb_r[i]]+left_num[fb_r[i]]);
						if (method ==0)
                            pr = 500;
						if (rand()%1000<pr)
						{
							while (ok[make_pair(fb_h[i],fb_r[i])].count(j)>0)
								j=rand_max(entity_num);
							train_kb(fb_h[i],fb_l[i],fb_r[i],fb_h[i],j,fb_r[i]);
						}
						else
						{
							while (ok[make_pair(j,fb_r[i])].count(fb_l[i])>0)
								j=rand_max(entity_num);
							train_kb(fb_h[i],fb_l[i],fb_r[i],j,fb_l[i],fb_r[i]);
						}
                		norm(relation_tmp[fb_r[i]]);
                		norm(entity_tmp[fb_h[i]]);
                		norm(entity_tmp[fb_l[i]]);
                		norm(entity_tmp[j]);
             		}
		            relation_vec = relation_tmp;
		            entity_vec = entity_tmp;
             	}
                cout<<"epoch:"<<epoch<<' '<<res<<endl;
                FILE* f2 = fopen(("relation2vec."+version).c_str(),"w");
                FILE* f3 = fopen(("entity2vec."+version).c_str(),"w");
                for (int i=0; i<relation_num; i++)
                {
                    for (int ii=0; ii<n; ii++)
                        fprintf(f2,"%.6lf\t",relation_vec[i][ii]);
                    fprintf(f2,"\n");
                }
                for (int i=0; i<entity_num; i++)
                {
                    for (int ii=0; ii<n; ii++)
                        fprintf(f3,"%.6lf\t",entity_vec[i][ii]);
                    fprintf(f3,"\n");
                }
                fclose(f2);
                fclose(f3);
            }
    }
    double res1;
    double calc_sum(int e1,int e2,int rel)
    {
        double sum=0;
        if (L1_flag)
        	for (int ii=0; ii<n; ii++)
            	sum+=fabs(entity_vec[e2][ii]-entity_vec[e1][ii]-relation_vec[rel][ii]);
        else
        	for (int ii=0; ii<n; ii++)
            	sum+=sqr(entity_vec[e2][ii]-entity_vec[e1][ii]-relation_vec[rel][ii]);
        return sum;
    }
    void gradient(int e1_a,int e2_a,int rel_a,int e1_b,int e2_b,int rel_b)
    {
        for (int ii=0; ii<n; ii++)
        {

            double x = 2*(entity_vec[e2_a][ii]-entity_vec[e1_a][ii]-relation_vec[rel_a][ii]);
            if (L1_flag)
            	if (x>0)
            		x=1;
            	else
            		x=-1;
            relation_tmp[rel_a][ii]-=-1*rate*x;
            entity_tmp[e1_a][ii]-=-1*rate*x;
            entity_tmp[e2_a][ii]+=-1*rate*x;
            x = 2*(entity_vec[e2_b][ii]-entity_vec[e1_b][ii]-relation_vec[rel_b][ii]);
            if (L1_flag)
            	if (x>0)
            		x=1;
            	else
            		x=-1;
            relation_tmp[rel_b][ii]-=rate*x;
            entity_tmp[e1_b][ii]-=rate*x;
            entity_tmp[e2_b][ii]+=rate*x;
        }
    }
    void train_kb(int e1_a,int e2_a,int rel_a,int e1_b,int e2_b,int rel_b)
    {
        double sum1 = calc_sum(e1_a,e2_a,rel_a);
        double sum2 = calc_sum(e1_b,e2_b,rel_b);
        if (sum1+margin>sum2)
        {
        	res+=margin+sum1-sum2;
        	gradient( e1_a, e2_a, rel_a, e1_b, e2_b, rel_b);
        }
    }
};

Train train;
void prepare()
{
	/* the file(entity2id.txt) has a structure like this
	   /m/06rf7	0
	   /m/0c94fn 1
	   that is mid and a uinique integer id assigned to it
	*/
    FILE* f1 = fopen("../data/entity2id.txt","r");


    /*  the file(relation2id.txt) has a structure like this
    	/people/appointed_role/appointment./people/appointment/appointed_by	0
		/location/statistical_region/rent50_2./measurement_unit/dated_money_value/currency	1
    */
	FILE* f2 = fopen("../data/relation2id.txt","r");

	int x;
	//read a line from the "entity2id.txt" file and store the mid in buff and it's unique id in x
	while (fscanf(f1,"%s%d",buf,&x)==2)
	{
		string st=buf;
		//entity2id is a map<string, int> that is it is used to create a mapping(dictionary) from mid to it's unique ID.
		entity2id[st]=x;
		//it creates a reverse mapping of the above mapping
		id2entity[x]=st;
		//counter to count the number of entities.
		entity_num++;
	}
	while (fscanf(f2,"%s%d",buf,&x)==2)
	{
		string st=buf;
		//same job is done for the relations thing as it is done for the entity thing.
		relation2id[st]=x;
		id2relation[x]=st;
		relation_num++;
	}
	/*	reading the training file "train.txt" and it's content looks like this
		/m/027rn	/m/06cx9	/location/country/form_of_government
		i.e. the entitties and the relation between them
	*/
    FILE* f_kb = fopen("../data/train.txt","r");
	while (fscanf(f_kb,"%s",buf)==1)
    {
    	//mid1 read into s1
        string s1=buf;
        fscanf(f_kb,"%s",buf);
        //mid2 read into s2
        string s2=buf;
        fscanf(f_kb,"%s",buf);
        //relationships read into s3
        string s3=buf;
        //checks to see that all these entities have unique ids.
        //if not print missing entity
        if (entity2id.count(s1)==0)
        {
            cout<<"miss entity:"<<s1<<endl;
        }
        if (entity2id.count(s2)==0)
        {
            cout<<"miss entity:"<<s2<<endl;
        }
        //do the same check for relations but if not present put it into the dictionary and increase the number of relationship count
        if (relation2id.count(s3)==0)
        {
            relation2id[s3] = relation_num;
            relation_num++;
        }
        //all the mappings are based on the unique ids assigned to the relationships and entities
        //mapping of relationship to left entity
        left_entity[relation2id[s3]][entity2id[s1]]++;
        //maping of relationship to  right entity
        right_entity[relation2id[s3]][entity2id[s2]]++;
        //add this current relation for training purposes, the train class uses dictionaries to save the left side and right
        //side entities as well as the realtionship
        train.add(entity2id[s1],entity2id[s2],relation2id[s3]);
    }
    //looping through all relationship ids
    for (int i=0; i<relation_num; i++)
    {

    	double sum1=0,sum2=0;
    	//looping through all the left side entities related to this relation
    	for (map<int,int>::iterator it = left_entity[i].begin(); it!=left_entity[i].end(); it++)
    	{
    		// this vatriable is used for counting the number of unique ids linked with this relation
    		sum1++;
    		// this variable is used for counting the total number of ids linked with this relation
    		sum2+=it->second;
    	}
    	//this dictionary keeps a count of the average number of times an entity has appeared in this relation with id i
    	left_num[i]=sum2/sum1;
    }
    //looping through all relationship ids
    for (int i=0; i<relation_num; i++)
    {
    	double sum1=0,sum2=0;
    	//doing the same stuff for the right hand side entity
    	for (map<int,int>::iterator it = right_entity[i].begin(); it!=right_entity[i].end(); it++)
    	{
    		sum1++;
    		sum2+=it->second;
    	}
    	//this dictionary keeps a count of the average number of times an entity has appeared in this relation with id i
    	right_num[i]=sum2/sum1;
    }
    //printing the relationship number and the entity numbers
    cout<<"relation_num="<<relation_num<<endl;
    cout<<"entity_num="<<entity_num<<endl;
    fclose(f_kb);
}

int ArgPos(char *str, int argc, char **argv) {
  /*
  	this function is used for locating a particular sting(str) in the arguments passed while running this code(argv)
  */
  int a;
  for (a = 1; a < argc; a++) if (!strcmp(str, argv[a])) {
    if (a == argc - 1) {
      printf("Argument missing for %s\n", str);
      exit(1);
    }
    return a;
  }
  return -1;
}

int main(int argc,char**argv)
{
    srand((unsigned) time(NULL));
    //-size : the embedding size k, d
    //-rate : learing rate
    //-method: 0 - unif, 1 - bern
    int method = 1;
    int n = 100; // dimension of the embedding space
    double rate = 0.001;
    double margin = 1;
    int i;
    if ((i = ArgPos((char *)"-size", argc, argv)) > 0) n = atoi(argv[i + 1]); // find if size is provided as an argument and if so store it's value in n
    if ((i = ArgPos((char *)"-margin", argc, argv)) > 0) margin = atoi(argv[i + 1]); // similarly if margin is mentioned store it in marge
    if ((i = ArgPos((char *)"-method", argc, argv)) > 0) method = atoi(argv[i + 1]); // similarly for method
    cout<<"size = "<<n<<endl;
    cout<<"learing rate = "<<rate<<endl;
    cout<<"margin = "<<margin<<endl;
    if (method)
        version = "bern";
    else
        version = "unif";
    cout<<"method = "<<version<<endl;
    //created all the dictionaries, load all the relationships in the train class and then we will start training
    prepare();
    train.run(n,rate,margin,method);
}

