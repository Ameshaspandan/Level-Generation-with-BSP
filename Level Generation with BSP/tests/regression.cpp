#include <vector>
#include <iostream>
#include <algorithm>
#include <random>
#include <string>
#include <sstream>
#include <cassert>
#include <set>
#include <cctype>
#include <map>
#define private public
#include "../Board.h"
#undef private
#include "../Board.cpp"

void partition(Board& b, int kind) {
    auto* original=b.rectangles.front();
    auto all=*original->GetCells(); original->GetCells()->clear(); delete original; b.rectangles.clear();
    std::vector<std::vector<Cell*>*> groups;
    int count=kind==0?3:4;
    for(int i=0;i<count;++i) groups.push_back(new std::vector<Cell*>);
    for(auto* c:all) {
        int index=kind==0 ? (c->Y()<2?0:c->Y()<5?1:2) :
            (c->Y()<3?0:c->X()<1?1:c->X()<4?2:3);
        groups[index]->push_back(c);
    }
    for(int i=0;i<count;++i) {auto* r=new Rectangle(groups[i]);r->SetColor(i);b.rectangles.push_back(r);}
    for(auto* r:b.rectangles) r->FindNeigbers();
    b.findRegion();
}
void verify(Board& b, bool all) {
    assert(b.rectangles.size()==(all?25:13));
    assert(b.regions.size()==(all?10:5));
    int a[8]={};
    std::set<Cell*> seen;
    for(auto* r:b.rectangles) {
        assert(r->Area()==r->Width()*r->Height());
        for(auto* c:*r->GetCells()) {assert(c->GetRectangle()==r);assert(seen.insert(c).second);}
    }
    assert(seen.size()==b.width*b.height);
    for(auto* r:b.regions) {
        auto m=r->GetMechanicLayer1();
        int id=-1;
        for(int i=0;i<6;++i) if(m==b.m[i]) id=i;
        assert(id>=0);a[id]+=r->Area();
        if(id==2) assert(b.isM2Candidate(r));
        if(id==3 || id==5) assert(b.isM3Candidate(r));
        if(id==4) assert(b.isM4Candidate(r));
        if(id==5) assert(r->IsEvenRectangle());
        if(id==1) for(auto* rectangle:r->GetConvex()) for(int i=0;i<rectangle->CountTopNeighbors();++i) {
            auto* n=rectangle->GetTopNeighbor(i);
            assert(n->GetRegion()==r || n->GetMechanicLayer1()=="#" || n->GetMechanicLayer1()=="A");
        }
        if(r->GetMechanicLayer0()=="AA") {assert(id!=0 && id!=4);a[6]+=r->Area();}
        if(r->GetMechanicLayer2()=="AAA") {assert(id!=0 && id!=1);a[7]+=r->Area();}
    }
    assert(a[0]>=5 && a[1]>=15 && a[1]<=25 && a[2]>=5 && a[5]>=4);
    assert(a[7]>=20 && a[7]<=50);
    if(all) assert(a[3]>=5 && a[4]>=5 && a[6]>=20 && a[6]<=50);
    else assert(a[3]==0 && a[4]==0 && a[6]==0);
    std::ostringstream output;output<<b;
    std::istringstream encoded(output.str());std::string part;
    std::getline(encoded,part,'/');
    for(int layer=0;layer<3;++layer) {
        assert(bool(std::getline(encoded,part,'/')));
        std::vector<std::string> values(b.width*b.height);
        size_t pos=0;int cursor=0;
        while(pos<part.size()) {
            std::string mechanic;
            while(pos<part.size() && !std::isdigit(static_cast<unsigned char>(part[pos]))) mechanic+=part[pos++];
            assert(!mechanic.empty() && pos<part.size());
            int w=part[pos++]-'0',h=0;
            while(pos<part.size() && std::isdigit(static_cast<unsigned char>(part[pos]))) h=h*10+part[pos++]-'0';
            assert(w>0 && h>0);
            while(cursor<values.size() && !values[cursor].empty()) ++cursor;
            int x=cursor%b.width,y=cursor/b.width;
            assert(x+w<=b.width && y+h<=b.height);
            for(int yy=y;yy<y+h;++yy) for(int xx=x;xx<x+w;++xx) {
                auto& v=values[yy*b.width+xx];assert(v.empty());v=mechanic;
            }
        }
        for(int i=0;i<values.size();++i) {
            auto* c=b.cells[i];
            assert(values[i]==(layer==0?c->Layer0():layer==1?c->Layer1():c->Layer2()));
        }
    }
}

void verifySymmetry(Board& b) {
    std::map<Rectangle*,Rectangle*> rectangleMirrors;
    std::map<Region*,Region*> regionMirrors;
    for(int y=0;y<b.height;++y) for(int x=0;x<b.width;++x) {
        auto* a=b.getCell(x,y);auto* other=b.getCell(b.width-1-x,y);
        assert(a->Layer0()==other->Layer0() && a->Layer1()==other->Layer1() && a->Layer2()==other->Layer2());
        auto* r=a->GetRectangle();auto* mirror=other->GetRectangle();
        if(rectangleMirrors.count(r)) assert(rectangleMirrors[r]==mirror);
        rectangleMirrors[r]=mirror;
        auto* region=r->GetRegion();auto* mirrorRegion=mirror->GetRegion();
        if(regionMirrors.count(region)) assert(regionMirrors[region]==mirrorRegion);
        regionMirrors[region]=mirrorRegion;
        assert(r->Width()==mirror->Width() && r->Height()==mirror->Height());
        assert(region->Area()==mirrorRegion->Area());
    }
    for(auto pair:rectangleMirrors) assert(rectangleMirrors[pair.second]==pair.first);
    for(auto pair:regionMirrors) assert(regionMirrors[pair.second]==pair.first);
    for(auto* region:b.regions) {
        std::set<Cell*> reached;
        std::vector<Cell*> queue{region->GetConvex().front()->BaseCell()};
        reached.insert(queue.front());
        for(size_t i=0;i<queue.size();++i) {
            auto* c=queue[i];
            for(auto offset:std::vector<std::pair<int,int>>{{-1,0},{1,0},{0,-1},{0,1}}) {
                int x=c->X()+offset.first,y=c->Y()+offset.second;
                if(x<0 || x>=b.width || y<0 || y>=b.height) continue;
                auto* neighbor=b.getCell(x,y);
                if(neighbor->GetRectangle()->GetRegion()==region && reached.insert(neighbor).second) queue.push_back(neighbor);
            }
        }
        assert(reached.size()==region->Area());
    }
}
void symmetryTests() {
    {Board b(9,9);bool rejected=false;try{b.LevelGenerate(13,4,false,true,false,true,true,false,true,true);}catch(const std::invalid_argument&){rejected=true;}assert(rejected);}
    for(auto dims:std::vector<std::pair<int,int>>{{9,9},{9,10},{9,11},{8,11}}) {
        for(int count:{1,2,3,12,13,dims.first*dims.second-1,dims.first*dims.second}) {
            Board b(dims.first,dims.second);
            b.rng.seed(count);
            bool built=false;
            for(int attempt=0;attempt<100 && !built;++attempt) {b.resetRectangle();built=b.splitSymmetric(count);}
            assert(built && b.rectangles.size()==count);
            assert(b.meshSymmetric(count));
            verifySymmetry(b);
        }
        for(int profile=0;profile<3;++profile) for(int run=0;run<10;++run) {
            bool all=profile==2;
            Board b(dims.first,dims.second);
            b.rng.seed(dims.first*100000+dims.second*1000+profile*100+run);
            try { b.LevelGenerate(all?25:13,all?10:5,profile!=1,true,all,profile!=0,true,all,true,true); }
            catch(const std::exception& e) {std::cerr<<"FAILED symmetry "<<dims.first<<"x"<<dims.second<<" profile "<<profile<<" run "<<run<<": "<<e.what()<<std::endl;throw;}
            verifySymmetry(b);
            if(profile!=1) verify(b,all);
            else {
                int areas[8]={};
                for(auto* r:b.regions) {
                    auto mechanic=r->GetMechanicLayer1();
                    for(int i=0;i<6;++i) if(mechanic==b.m[i]) areas[i]+=r->Area();
                    if(mechanic=="B") assert(b.isM2Candidate(r));
                    if(mechanic=="D") assert(b.isM4Candidate(r));
                    if(mechanic=="E") assert(b.isM3Candidate(r) && r->IsEvenRectangle());
                    if(r->GetMechanicLayer2()=="AAA") {assert(mechanic!="A");areas[7]+=r->Area();}
                }
                assert(areas[0]==0 && areas[1]>=15 && areas[1]<=25 && areas[2]>=5 &&
                    areas[3]==0 && areas[4]>=5 && areas[5]>=4 && areas[7]>=20 && areas[7]<=50);
                assert(b.rectangles.size()==13 && b.regions.size()==5);
            }
        }
        std::cout<<"PASS symmetry "<<dims.first<<"x"<<dims.second<<" (30 boards, three mechanic profiles)"<<std::endl;
    }
}


int main() {
    symmetryTests();
    for(int run=0;run<200;++run) {
        Board b(9,11);
        while(b.rectangles.size()<99) {
            auto it=std::find_if(b.rectangles.begin(),b.rectangles.end(),[](Rectangle* r){return r->Area()>1;});
            assert(it!=b.rectangles.end());
            b.rectangles.push_back((*it)->Split());
            std::set<Cell*> seen;
            for(auto* r:b.rectangles) {
                assert(r->Area()==r->Width()*r->Height());
                int x0=r->BaseCell()->X(),y0=r->BaseCell()->Y();
                for(int y=0;y<r->Height();++y) for(int x=0;x<r->Width();++x) {
                    auto* c=r->getCell(x,y);assert(c->X()==x+x0 && c->Y()==y+y0);
                    assert(c->GetRectangle()==r && seen.insert(c).second);
                }
            }
            assert(seen.size()==99);
        }
        for(auto* r:b.rectangles) {r->FindNeigbers();assert(r->CountTopNeighbors()==(r->BaseCell()->Y()>0));assert(r->CountLeftNeighbors()==(r->BaseCell()->X()>0));}
    }
    std::cout<<"PASS 200 full BSP subdivisions\n";
    {Board b(9,9);bool stopped=false;try{b.LevelGenerate(2,2,true,false,false,false,false,false,false);}catch(const std::runtime_error&){stopped=true;}assert(stopped);}
    { Board b(9,9);partition(b,0);assert(b.paint(false,true,false,false,false,false,false)); }
    { Board b(8,11);partition(b,1);assert(b.paint(false,true,false,false,true,false,false)); }
    { Board b(9,9);partition(b,0);bool rejected=false;try{b.mesh(4);}catch(const std::invalid_argument&){rejected=true;}assert(rejected); }
    for(auto dims:std::vector<std::pair<int,int>>{{9,9},{9,10},{9,11},{8,11}}) {
        for(bool all:{false,true}) for(int i=0;i<10;++i) {
            Board b(dims.first,dims.second);
            b.LevelGenerate(all?25:13,all?10:5,true,true,all,all,true,all,true);
            verify(b,all);
        }
        std::cout<<"PASS size "<<dims.first<<"x"<<dims.second<<" (20 generated boards and serialization round trips)\n";
    }
    for(auto dims:std::vector<std::pair<int,int>>{{8,10},{10,9},{0,0},{3,3}}) {
        bool rejected=false;try{Board b(dims.first,dims.second);}catch(const std::invalid_argument&){rejected=true;}assert(rejected);
    }
    {Board b(9,9);bool rejected=false;try{b.LevelGenerate(82,5,true,true,false,false,true,false,true);}catch(const std::invalid_argument&){rejected=true;}assert(rejected);}
    {Board b(9,9);bool rejected=false;try{b.LevelGenerate(13,5,false,false,false,false,false,false,false);}catch(const std::invalid_argument&){rejected=true;}assert(rejected);}
    {Rectangle a(2,2),b(1,1);Region r;r.AddRectangle(&a);r.AddRectangle(&a);assert(r.Size()==1);
     auto* cell=a.getCell(1,1);a.ExitCell(cell);b.AddCell(cell);assert(cell->GetRectangle()==&b);assert(r.Area()==3);
     assert(b.Width()==2 && b.Height()==2);assert(b.getCell(1,1)==cell);r.RemoveRecangle(&a);assert(a.GetRegion()==nullptr);}
    std::cout<<"PASS regression cases, invalid settings, ownership and region area\n";
}





