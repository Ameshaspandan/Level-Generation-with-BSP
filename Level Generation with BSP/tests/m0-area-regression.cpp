#include <vector>
#include <iostream>
#include <algorithm>
#include <random>
#include <string>
#include <sstream>
#include <cassert>
#include <functional>
#define private public
#include "../Board.h"
#undef private
#include "../Board.cpp"

void makeRegions(Board& b, const std::function<int(int,int)>& color) {
    auto* base=b.rectangles.front();
    base->GetCells()->clear();delete base;b.rectangles.clear();
    for(auto* cell:b.cells) {
        auto* r=new Rectangle(new std::vector<Cell*>{cell});
        r->SetColor(color(cell->X(),cell->Y()));b.rectangles.push_back(r);
    }
    for(auto* r:b.rectangles) r->FindNeigbers();
    b.findRegion();
}
int m0Area(Board& b) {
    int result=0;
    for(auto* c:b.cells) if(c->Layer1()=="#") ++result;
    return result;
}
bool paint(Board& b,bool symmetry,bool enabled=true) {
    return symmetry ? b.paintSymmetric(enabled,true,false,false,false,false,false)
        : b.paint(enabled,true,false,false,false,false,false);
}
int main() {
    // Non-symmetric smallest-first selection: minimum, maximum and overshoot.
    for(int amount:{5,10,11}) {
        Board b(9,9);
        makeRegions(b,[&](int x,int y){int i=y*9+x;return i<amount?0:i<amount+18?1:2;});
        assert(paint(b,false)==(amount<=10));
        if(amount<=10) {assert(m0Area(b)==amount);assert(b.regions.front()->GetMechanicLayer1()=="#");}
        else assert(m0Area(b)==0);
    }
    {
        Board b(9,9);
        makeRegions(b,[](int x,int y){int i=y*9+x;return i<4?0:i<11?1:i<29?2:3;});
        assert(!paint(b,false));assert(m0Area(b)==4); // 4+7 must not become 11.
    }
    {
        Board b(9,9);
        makeRegions(b,[](int x,int y){int i=y*9+x;return i<2?0:i<5?1:i<23?2:3;});
        assert(paint(b,false));assert(m0Area(b)==5);
    }
    // Mirrored regions of 5+5 are permitted; 6+6 are not.
    for(int pairArea:{5,6}) {
        Board b(8,11);
        makeRegions(b,[&](int x,int y){
            if(x==0 && y<pairArea) return 0;
            if(x==7 && y<pairArea) return 1;
            if(x>0 && x<7 && y<3) return 2;
            return 3;
        });
        b.rng.seed(42);
        assert(paint(b,true)==(pairArea==5));
        if(pairArea==5) assert(m0Area(b)==10);
    }
    // A self-mirrored center region can have exactly the minimum area.
    {
        Board b(9,9);
        makeRegions(b,[](int x,int y){
            if(x==4 && y<5) return 0;
            if(x<4 && y<2) return 1;
            if(x>4 && y<2) return 2;
            return 3;
        });
        assert(paint(b,true));assert(m0Area(b)==5);
    }
    for(bool symmetry:{false,true}) {
        Board b(9,9);
        makeRegions(b,[](int,int y){return y<2?0:1;});
        assert(paint(b,symmetry,false));assert(m0Area(b)==0);
    }
    std::cout<<"PASS exact 5/10 boundaries, smallest-first accumulation, over-limit rejection, paired area and M0 disabled\n";

    for(auto size:std::vector<std::pair<int,int>>{{9,9},{9,10},{9,11},{8,11}}) {
        for(bool symmetry:{false,true}) for(int run=0;run<10;++run) {
            Board b(size.first,size.second);
            b.rng.seed(size.first*10000+size.second*100+run);
            bool accepted=false;
            // Bound the test itself independently of the application's retry policy.
            for(int attempt=0;attempt<3000 && !accepted;++attempt) {
                b.resetRectangle();
                if(symmetry) {
                    if(!b.splitSymmetric(13)) continue;
                    for(int grouping=0;grouping<16 && !accepted;++grouping)
                        if(b.meshSymmetric(5)) accepted=b.paintSymmetric(true,true,false,false,true,false,true);
                } else {
                    while(b.rectangles.size()<13) {
                        std::vector<Rectangle*> candidates;
                        for(auto* r:b.rectangles) if(r->Area()>1) candidates.push_back(r);
                        auto* r=candidates[b.randomInt(0,static_cast<int>(candidates.size())-1)];
                        b.rectangles.push_back(r->Split());
                    }
                    for(auto* r:b.rectangles) r->FindNeigbers();
                    b.mesh(5);accepted=b.paint(true,true,false,false,true,false,true);
                }
            }
            assert(accepted);assert(m0Area(b)>=5 && m0Area(b)<=10);
            if(symmetry) for(int y=0;y<b.height;++y) for(int x=0;x<b.width;++x)
                assert(b.getCell(x,y)->Layer1()==b.getCell(b.width-1-x,y)->Layer1());
        }
        std::cout<<"PASS "<<size.first<<"x"<<size.second<<" (10 boards per mode)\n";
    }
}


