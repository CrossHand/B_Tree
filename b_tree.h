#include"iostream"
#include"queue"
using namespace std;
//定义bTree节点类,默认节点关键值数N=3;
#define N 3
class b_Node
{
public:
	b_Node *parent;
	int key[N+1];//存放关键值
	int keynum;
	b_Node *ptr[N+1];//分裂时需要用的,假设上一层有一个关键值，对应下一层关键值有两个，如果上一层有两个则下一层有三个，上一层最多3，所以下层最多4个
	b_Node();
	void show();
};
b_Node::b_Node()
{
	parent = 0;
	keynum = 0;
	for (int i = 0; i < N+1; i++)
		ptr[i] = 0;
}

//定义一个类，表示查找结果,因为bTree不再像二叉树一样简单返回一个节点就可以，还要返回在节点的第几个位置
class result
{
public:
	b_Node *p;
	int i;
	result();
	bool tag;//表示成功找到或者未找到
};
result::result()
{
	p = 0;
	i = 0;
}
//定义bTree类
class bTree
{
public:
	b_Node *root;
	bTree();
	result searchKey(b_Node *T,int key);
	bool insert(int key_in);
	void split(b_Node *sp);
	void print_tree();
};
bTree::bTree()
{
	root = 0;
}


int search(b_Node *P, int key)
{
	int i = 1;
	if (key < P->key[i])
		return 0;
	else if (key>P->key[P->keynum])
		return  P->keynum;
	for (int j = 1; j <= P->keynum; j++)
	{
		if (key >= P->key[j] && key < P->key[P->keynum])
			return j;
	}
}

//找某个数在树中的位置，如果找不到，返回应该插入的位置
result bTree::searchKey(b_Node *T,int key)
{
	result re;
	int i=0;
	bool found = false;
	b_Node *p=T;
	b_Node *q = 0;
	
	while(p != 0&&!found)
	{
		p->parent = q;
		i = search(p, key);
		if (i > 0 && p->key[i] == key) found = true;
		else
		{
			q = p; 
			p = p->ptr[i];//继续往下一层找直到为空
			//p->parent = q;
		}

	}
	if (found)
	{
		re.p = q;
		re.i = i;
		re.tag = 1;//表示已经找到
	}
	else
	{
		re.p = q;
		re.i = i+1;
		re.tag = 0;
	}
	return re;
}
bool bTree::insert(int key_in)
{
    //先搜索位置
	
	if (root == 0) { root = new b_Node; root->key[1] = key_in; root->keynum++; return true;}
	result re = searchKey(root, key_in);
	if (re.tag)
	{
		cout << "the data is existed!" << endl;
		return false;
	}
	else
	{
		if (re.i <= N)
		{
			if (re.p->keynum + 1 > N)    //考虑顺序需要改变的情况
			{
				re.p->key[0] = re.p->key[re.p->keynum];
				for (int j = re.p->keynum; j > re.i; j--)
					re.p->key[j] = re.p->key[j - 1];
				re.p->key[re.i] = key_in;
				re.p->keynum++;
			}
			else
			{
				for (int j = re.p->keynum+1; j > re.i; j--)
					re.p->key[j] = re.p->key[j - 1];
				re.p->key[re.i] = key_in;
				re.p->keynum++;

			}
		}
		else
		{
			re.p->key[0] = key_in;//暂存
			re.p->keynum++;
		}
	}
	if (re.p->keynum <=N) return true;
	else
	{
	   split(re.p);


	}

}
void bTree::split(b_Node *sp)
{
   //首先如果上一层不为空并且还没有满的时候，考虑向上进位
	if (sp->parent != 0 && sp->parent->keynum != N)
	{
		int loc = sp->parent->keynum+1;
		sp->parent->key[loc] = sp->key[2];
		sp->parent->keynum++;
		for (int k = 1; k <= sp->parent->keynum; k++) //找到往上进位的位置
		{
			if (sp->key[2] > sp->parent->key[k] && sp->key[2]< sp->parent->key[k+1])
			{
				sp->parent->key[k+1] = sp->key[2];
				loc = k+1;
				break;
			}
		}
      //将sp->key[2]左右的元素分配好
		sp->parent->ptr[loc - 1] = new b_Node;
		sp->parent->ptr[loc-1]->key[1]=sp->key[1];
		sp->parent->ptr[loc - 1]->keynum++;
		sp->parent->ptr[loc] = new b_Node;
		for (int l = 3; l <= sp->keynum-1; l++)
		{
			sp->parent->ptr[loc]->key[l-2]=sp->key[l];
			sp->parent->ptr[loc]->keynum++;
		}
		sp->parent->ptr[loc]->key[sp->keynum-2] = sp->key[0];//0里暂存了最大的元素
		sp->parent->ptr[loc]->keynum++;
		//sp->keynum--;
		
	}
	else //否则自身向下分裂
	{
		int M = (N + 1) / 2;
		
		for (int i = 1; i < M; i++)
		{
			sp->ptr[0] = new b_Node;
			sp->ptr[0]->key[i] = sp->key[i];
			sp->ptr[0]->keynum++;
		}
		for (int j = M + 1; j <= sp->keynum-1; j++)
		{
			sp->ptr[1] = new b_Node;
			sp->ptr[1]->key[j-2] = sp->key[j];//有问题，下标从0开始
			sp->ptr[1]->keynum++;
		}
		sp->ptr[1]->key[sp->keynum-M] = sp->key[0];
		sp->ptr[1]->keynum++;
		sp->key[1] = sp->key[M];//剩下元素要清空
		sp->keynum = 1;
	}
}

void bTree::print_tree()
{ 
	queue<b_Node*> stack1,stack2;//定义两个队列是为了方便判断每一层什么时候输出结束时，输出换行符
	b_Node *p=0,*q=0;
	stack1.push(root);
	while (!stack1.empty() || !stack2.empty())//两个队列交换存储每一层的数据，stack1先存第1层，stack存第2层，然后stack1清空后再存第三层，以此类推，两个stack一个在输出时，另一个就在输入
	{
		while (!stack1.empty())
		{
			p = stack1.front();
			p->show();

			for (int i = 0; i <= N; i++)
			{
				if (p->ptr[i])
					stack2.push(p->ptr[i]);
			}
			cout << "        ";
			stack1.pop();

		}
		cout << endl;
		while (!stack2.empty())
		{
			q = stack2.front();
			q->show();
			for (int i = 0; i <= N; i++)
			{
				if (q->ptr[i])
					stack1.push(q->ptr[i]);
			}
			cout << "        ";
			stack2.pop();
		}
		cout << endl;
	}
}

void b_Node::show()
{
	for (int i = 1; i <= keynum; i++)
		cout <<key[i]<<"   ";
  
}
