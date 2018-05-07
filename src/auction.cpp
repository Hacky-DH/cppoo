#include<iostream>
#include<fstream>
#include<cstdlib>
#include<iomanip>
#include<string>
#include<vector>

using namespace std;

//商品类
class Product
{
private:
	int id;
	string name;//名称
	double price;//保留价
	string desc;//描述信息

	friend class Monitor;
	friend class Surface;
};

//竞拍者类
class Customer
{
private:
	int id;
	string name;//名称

	friend class Monitor;
	friend class Surface;
};

//一次竞拍信息
class AucInfo
{
private:
	int num;//竞价序号
	double price;//竞价价格

	friend class Result;
	friend class Monitor;
};

//竞拍结果
class Result
{
private:
	int proId;//商品ID
	int custId;//竞拍者ID
	vector<AucInfo> info;//竞价序号信息的集合
	//enum{NONE,MAKEADEAL,PASS}state;//物品竞拍状态，待拍卖或正在拍卖，成交，流拍

	friend class Monitor;
};

//监控类
class Monitor
{
private:
	vector<Product> prods;//商品
	vector<Customer> custs;//竞拍者
	vector<Result> res;//竞价结果
public:
	Monitor(){}
	~Monitor(){}
	void Entrust(Product &pro); //委托商品
	void Notice(ostream& out=cout,bool end=false);//公告商品
	bool Auction(int proId, int custId, double price);//竞拍商品
	void StatAuc(ostream& out=cout);//统计竞拍结果

	friend class Surface;
};

void Monitor::Entrust(Product &pro)
{
	prods.push_back(pro);
}

void Monitor::Notice(ostream& out,bool end)
{
	out.setf(ios::left);
	out<<setw(10)<<"商品编号"<<setw(10)<<"商品名称"<<setw(8)<<"保留价"<<setw(12)
		<<"目前最高价"<<setw(8)<<"出价者"<<setw(10)<<"商品描述"<<endl;
	string name;
	if(end)
		name="该商品流拍。";
	else
		name="***";
	for(size_t i=0;i<prods.size();++i)
	{
		out<<setw(10)<<prods[i].id<<setw(10)<<prods[i].name<<setw(8)<<prods[i].price;
		bool fg=false;
		for(size_t j=0;j<res.size();++j)
		{
			if(prods[i].id==res[j].proId)
			{
				double max=0;
				int custID=-1;
				for(size_t k=0;k<res[j].info.size();++k)
				{
					if(max<res[j].info[k].price)
					{
						max=res[j].info[k].price;
						custID=res[j].custId;
					}
				}
				for(size_t l=0;l<custs.size();++l)
				{
					if(custs[l].id==custID)
					{
						name=custs[l].name;
					}
				}
				out<<setw(12)<<max<<setw(8)<<name;
				fg=true;
				break;
			}
		}
		if(!fg)
			out<<setw(12)<<0<<setw(8)<<name<<endl;
		else
			out<<setw(10)<<prods[i].desc<<endl;
	}
}

bool Monitor::Auction(int proId, int custId, double price)
{
	static int num=1;
	for(size_t i=0;i<prods.size();++i)
	{
		if(prods[i].id==proId&&prods[i].price>=price)
			return false;
	}
	bool find=false;
	int pos;
	for(size_t i=0;i<res.size();++i)
	{
		if(res[i].proId==proId&&res[i].custId==custId)
		{
			for(size_t j=0;j<res[i].info.size();++j)
			{
				if(res[i].info[j].price>=price)
					return false;
			}
			find=true;
			pos=i;
			break;
		}
	}
	AucInfo ai;
	if(find)
	{
		ai.num=num++;
		ai.price=price;
		res[pos].info.push_back(ai);
	}
	else
	{
		Result rt;
		rt.proId=proId;
		rt.custId=custId;
		ai.num=num++;
		ai.price=price;
		rt.info.push_back(ai);
		res.push_back(rt);
	}
	return true;
}

void Monitor::StatAuc(ostream& out)
{
	Notice(out,true);
}

//界面类
class Surface
{
	Monitor mtr;
public:
	void menu();
	void Entrust();
	void Auction();
};

void Surface::menu()
{
	#define space "\t\t\t"
	cout<<"\n\n\t\t......**进入拍卖系统**......\n"
		<<space<<"1. 委托商品\n"
		<<space<<"2. 公告商品\n"
		<<space<<"3. 竞拍商品\n"
		<<space<<"4. 拍卖结果\n"
		<<space<<"5. 退出系统\n";
	cout<<"请选择菜单：";
	char c;
	cin>>c;
	switch(c)
	{
	case '1': Entrust();break;
	case '2': mtr.Notice();break;
	case '3': Auction();break;
	case '4': mtr.StatAuc();break;
	case '5': system("PAUSE");return;
	}
	system("PAUSE");
	system("cls");
	menu();
}

void Surface::Entrust()
{
	static int num=1;
	Product newpd;
	cout<<"请输入商品名称：";
	cin>>newpd.name;
	cout<<"请输入保留价:";
	cin>>newpd.price;
	cout<<"请输入商品简介：\n";
	cin>>newpd.desc;
	newpd.id=num++;
	mtr.Entrust(newpd);
	cout<<"成功委托商品！"<<endl;
}

void Surface::Auction()
{
	if(mtr.prods.size()==0)
	{
		cout<<"目前没有商品可以竞价！"<<endl;
		return;
	}
	static int num=1;
	Customer c;
	cout<<"请输入竞价者的名称：";
	cin>>c.name;
	c.id=num++;
	mtr.custs.push_back(c);
	cout<<"目前存在的商品信息如下："<<endl;
	mtr.Notice();
	cout<<"请选择要商品的编号：";
	char cr;
	cin>>cr;
	size_t w=cr-'0'-1;
	if(w>mtr.prods.size())
	{
		cout<<"输入数据有误！";
		return;
	}
	cout<<"请输入竞价价格：";
	double p;
	cin>>p;
	if(mtr.Auction(mtr.prods[w].id,c.id,p))
	{
		cout<<"\n**恭喜竞价成功!**\n"<<endl;
	}
	else
	{
		cerr<<"\n对不起，您的出价无效!\n";
	}
}

int main()
{
	Surface sf;
	sf.menu();
	return 0;
}

