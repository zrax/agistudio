/*
 *  QT AGI Studio :: Copyright (C) 2000 Helen Zommer
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  A special exception to the GNU General Public License is made for
 *  linking to a non-GPL library, specifically, the Windows version of
 *  Qt.  For more information on Qt, see www.trolltech.com.
 */

#include "resources.h"
#include "game.h"
#include "preview.h"
#include "picture.h"
#include "wutil.h"
#include "menu.h"

#include "logedit.h"

#include <stdio.h>

#include <qbuttongroup.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qfiledialog.h>
#include <qpainter.h>

#include "right_x.xpm"
#include "left_x.xpm"


//*****************************************
Preview::Preview( QWidget* parent, const char*  name ,ResourcesWin *res ):
  QWidgetStack(parent,name)
{

  setCaption("Preview");
  make_egacolors();

  resources_win = res;

  // Logic

  w_logic = new QWidget (this);
  w_logic->setMinimumSize(340,280);

  QBoxLayout *d =  new QVBoxLayout(w_logic,5);
  p_logic = new LogEdit( w_logic,0,0,res,true );
  d->addWidget(p_logic);
  QPushButton *edit = new QPushButton("Edit",w_logic);
  edit->setMaximumSize(80,60);
  connect(edit,SIGNAL(clicked()),SLOT(double_click()));
  d->addWidget(edit);

  // Sound

  w_sound = new QWidget (this);
  w_sound->setMinimumSize(340,280);

  QBoxLayout *d1 =  new QVBoxLayout(w_sound,20);
  QLabel *l1 = new QLabel("Preview is not available !\nDouble-click to listen.\nOr click the 'Listen' button.",w_sound,0);
  d1->addWidget(l1);
  QPushButton *listen = new QPushButton("Listen",w_sound);
  listen->setMaximumSize(80,60);
  connect(listen,SIGNAL(clicked()),SLOT(double_click()));
  d1->addWidget(listen);
  d1->addStretch();

  // Picture

  w_picture = new QWidget (this,0);
  w_picture->setMinimumSize(340,280);

  QBoxLayout *pbox =  new QVBoxLayout(w_picture,10);

  p_picture = new PreviewPicture(w_picture,0,this);
  p_picture->setFixedSize(MAX_W,MAX_HH);
  pbox->addWidget(p_picture);

  QBoxLayout *pbox1 =  new QHBoxLayout(pbox,10);

  visual = new QRadioButton("Visual",w_picture);
  visual->setChecked(true);
  pbox1->addWidget(visual);

  priority = new QRadioButton("Priority",w_picture);
  priority->setChecked(false);
  pbox1->addWidget(priority);

  p_picture->drawing_mode=0;

  QButtonGroup *bg = new QButtonGroup(w_picture);
  bg->hide();
  bg->insert(visual,0);
  bg->insert(priority,1);
  connect( bg, SIGNAL(clicked(int)), SLOT(change_mode(int)) );


  QBoxLayout *pbox2 =  new QHBoxLayout(pbox,10);
  QPushButton *pedit = new QPushButton("Edit",w_picture);
  pedit->setMaximumWidth(100);
  connect(pedit,SIGNAL(clicked()),SLOT(double_click()));
  pbox2->addWidget(pedit,AlignCenter);

  save = new QPushButton("Save as...",w_picture);
  save->setMaximumWidth(100);
  connect(save,SIGNAL(clicked()),SLOT(save_pic()));
  pbox2->addWidget(save,AlignCenter);

  formats = new QComboBox(w_picture);
  QStrList out =  QImageIO::outputFormats ();
  for (unsigned int k=0;k<out.count();k++ ){
    formats->insertItem(out.at(k));
  }
  pbox2->addWidget(formats);
  pbox->addStretch();

  // View

  w_view = new QWidget (this,0);
  w_view->setMinimumSize(340,240);

  QBoxLayout *vbox = new QVBoxLayout(w_view,10);

  int maxrow1=2,maxcol1=5;
  QGridLayout *grid1 = new QGridLayout( vbox, maxrow1,maxcol1, 1 );

  int i;
  for(i=0;i<maxcol1;i++){
    grid1->setColStretch(i,1);
    grid1->addColSpacing(i,1);
  }

  for(i=0;i<maxrow1;i++){
    grid1->setRowStretch(i,1);
    grid1->addRowSpacing(i,2);
  }


  int row=0;int col=0;
  QPixmap pright=QPixmap(right_x);
  QPixmap pleft=QPixmap(left_x);

  QLabel *looplabel = new QLabel("Loop:",w_view);
  grid1->addWidget(looplabel,row,col,AlignRight); col++;

  loopnum = new QLabel("0/0",w_view);
  grid1->addWidget(loopnum,row,col,AlignLeft);   col++;

  QPushButton *loopleft = new QPushButton(w_view);
  loopleft->setPixmap(pleft);
  connect( loopleft, SIGNAL(clicked()), SLOT(previous_loop()) );
  grid1->addWidget(loopleft,row,col,AlignRight);    col++;

  QPushButton *loopright = new QPushButton(w_view);
  loopright->setPixmap(pright);
  connect( loopright, SIGNAL(clicked()), SLOT(next_loop()) );
  grid1->addWidget(loopright,row,col,AlignLeft);    col++;

  QPushButton *vedit = new QPushButton("Edit",w_view);
  vedit->setMaximumWidth(100);
  connect(vedit,SIGNAL(clicked()),SLOT(double_click()));
  grid1->addWidget(vedit,row,col,AlignCenter); col++;

  row++;col=0;

  QLabel *cellabel = new QLabel("Cel:",w_view);
  grid1->addWidget(cellabel,row,col,AlignRight);  col++;

  celnum = new QLabel("0/0",w_view);
  grid1->addWidget(celnum,row,col,AlignLeft);    col++;

  QPushButton *celleft = new QPushButton(w_view);
  celleft->setPixmap(pleft);
  connect( celleft, SIGNAL(clicked()), SLOT(previous_cel()) );
  grid1->addWidget(celleft,row,col,AlignRight);     col++;

  QPushButton *celright = new QPushButton(w_view);
  celright->setPixmap(pright);
  connect( celright, SIGNAL(clicked()), SLOT(next_cel()) );
  grid1->addWidget(celright,row,col,AlignLeft); col++;

  QPushButton *anim = new QPushButton("Animate",w_view);
  connect(anim,SIGNAL(clicked()),SLOT(animate_cb()));
  grid1->addWidget(anim,row,col,AlignCenter); col++;

  p_view = new PreviewView(w_view,0,this);
  p_view->setMinimumSize(64,64);
  p_view->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ));
  vbox->addWidget(p_view);

  description = new QMultiLineEdit(w_view);
  description->setReadOnly(true);
  description->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ));
  vbox->addWidget(description);
  vbox->addStretch();

  // Build the widget stack

  addWidget( w_picture );
  addWidget( w_view );
  addWidget( w_sound );
  addWidget( w_logic );

  setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ));
  adjustSize();
  animate=NULL;
}

//*****************************************
void Preview::open(int ResNum,int type)
{
  switch(type){
  case LOGIC:
    if(animate){ animate->closeall(); animate=NULL; }
    raiseWidget( w_logic );
    p_logic->open(ResNum);
    break;
  case VIEW:
    raiseWidget( w_view );
    p_view->draw(ResNum);
    break;
  case PICTURE:
    if(animate){ animate->closeall(); animate=NULL; }
    raiseWidget( w_picture );
    p_picture->draw(ResNum);
    break;
  case SOUND:
    if(animate){ animate->closeall(); animate=NULL; }
    raiseWidget( w_sound );
    break;
  }

  show();
}

//*****************************************
void Preview::change_mode(int mode)
{

  p_picture->drawing_mode=mode;
  p_picture->update();

}

//******************************************************
void Preview::double_click()
{

  resources_win->select_resource(resources_win->list->currentItem());

}
//******************************************************
void Preview::previous_loop()
{
  
  if(p_view->view->CurLoop>0){
    p_view->view->CurLoop--;
    p_view->view->CurCel=0;
    showlooppar();
    showcelpar();
    p_view->update();
  }
}
//******************************************************
void Preview::next_loop()
{
  if(p_view->view->CurLoop<p_view->view->NumLoops-1){
    p_view->view->CurLoop++;
    p_view->view->CurCel=0;
    showlooppar();
    showcelpar();
    p_view->update();
  }

}
//******************************************************
void Preview::previous_cel()
{
  if(p_view->view->CurCel>0){
    p_view->view->CurCel--;
    showcelpar();
    p_view->update();
  }

}
//******************************************************
void Preview::prev_cel_cycle()
{

  if(p_view->view->loops[p_view->view->CurLoop].NumCels<=1)return;
  if(p_view->view->CurCel>0){
    p_view->view->CurCel--;
  }
  else{
    p_view->view->CurCel=p_view->view->loops[p_view->view->CurLoop].NumCels-1;
  }
  showcelpar();
  p_view->update();

}
//******************************************************
void Preview::next_cel()
{

  if(p_view->view->CurCel<p_view->view->loops[p_view->view->CurLoop].NumCels-1){
    p_view->view->CurCel++;
    showcelpar();
    p_view->update();
  }

}
//******************************************************
void Preview::next_cel_cycle()
{
  if(p_view->view->loops[p_view->view->CurLoop].NumCels<=1)return;
  if(p_view->view->CurCel<p_view->view->loops[p_view->view->CurLoop].NumCels-1){
    p_view->view->CurCel++;
  }
  else{
    p_view->view->CurCel=0;
  }
  showcelpar();
  p_view->update();

}
//******************************************************
void Preview::showlooppar()
{

  sprintf(tmp,"%d/%d",p_view->view->CurLoop,p_view->view->NumLoops-1);
  loopnum->setText(tmp);

}
//******************************************************
void Preview::animate_cb()
{

  if(animate==NULL)animate=new Animate(0,0,this,0);
  animate->show();

}
//******************************************************
void Preview::showcelpar()
{

  sprintf(tmp,"%d/%d",p_view->view->CurCel,p_view->view->loops[p_view->view->CurLoop].NumCels-1);
  celnum->setText(tmp);

}
//******************************************************
void Preview::save_pic()
{

  const char *format = formats->currentText().latin1();

  QFileDialog *f = new QFileDialog(0,"Save",true);  
  sprintf(tmp,"*.%s",format);
  toLower(tmp);
  const char *filters[] = {tmp,"All files (*)",NULL};

  f->setFilters(filters);
  f->setCaption("Save view");
  f->setMode(QFileDialog::AnyFile);
  f->setDir(game->srcdir.c_str());
  if ( f->exec() == QDialog::Accepted ) {
    if ( !f->selectedFile().isEmpty() ){
      if(!p_picture->pixmap.save((char *)f->selectedFile().latin1(),format))
        menu->errmes("Couldn't save picture !");

    }    
  }  

}
//*****************************************
void Preview::deinit()
{

  if(animate){
    animate->closeall();
    animate=NULL;
  }
  resources_win->preview=NULL;
  if(window_list && window_list->isVisible())window_list->draw();
}


//*********************************************
void Preview::closeEvent( QCloseEvent *e )
{
  
  deinit();
  e->accept();

}
//*********************************************
void Preview::hideEvent( QHideEvent * )
{

  if(window_list && window_list->isVisible())window_list->draw();

}

//*********************************************
void Preview::showEvent( QShowEvent * )
{

  if(window_list && window_list->isVisible())window_list->draw();

}

//******************************************************

PreviewPicture::PreviewPicture(  QWidget* parent, const char*  name, Preview *p ):
  QWidget(parent,name)
{
  
  preview = p;
  pixmap = QPixmap(MAX_W,MAX_HH);
  ppicture = new BPicture();
  
}

//*****************************************
void PreviewPicture::draw(int ResNum)
{

  int err = game->ReadResource(PICTURE,ResNum);
  if(!err){
    ppicture->show(ResourceData.Data,ResourceData.Size);
    update();
  }

}

//*****************************************
void PreviewPicture::update()
{
  QPainter p(&pixmap); 
  byte **data;
  int x,y;
  byte c0=255,c;

  data = (drawing_mode)?ppicture->priority:ppicture->picture;
  for(y=0;y<MAX_HH;y++){
    for(x=0;x<MAX_W;x++){
      c=data[y][x];
      if(c!=c0){
        p.setPen(egacolor[c]);
        c0=c;
      }
      p.drawPoint(x,y);
    }
  }
  repaint();
}

//*****************************************
void PreviewPicture::paintEvent(QPaintEvent *)
{

  QPainter p(this);  
  p.drawPixmap( 0, 0, pixmap );

}
//******************************************************
PreviewView::PreviewView(  QWidget* parent, const char*  name, Preview *p ):
  QWidget(parent,name)
{
  
  preview = p;
  view = new View();
  pixmap = QPixmap();
  cur_w = cur_h = 0;
  pixsize = 2;
}

//*****************************************
void PreviewView::paintEvent(QPaintEvent *)
{
  QPainter p(this);  
  int W = preview->width();
  int x = (W-pixsize*cur_w*2)/2;    
  p.drawPixmap( x, 0, pixmap );
}

//*****************************************
void PreviewView::update()
{
  int x,y;

  if(!view->opened)return;
  
  int w = view->loops[view->CurLoop].cels[view->CurCel].width;
  int h = view->loops[view->CurLoop].cels[view->CurCel].height;
  bool mirror = (view->loops[view->CurLoop].mirror!=-1);
  byte *data = view->loops[view->CurLoop].cels[view->CurCel].data;

  if(cur_w != w || cur_h != h){
    pixmap.resize(w*pixsize*2,h*pixsize);
    cur_w=w;
    cur_h=h;
  }

  QPainter p(&pixmap);   
  
  if(mirror){      
    for(y=0;y<h;y++){
      for(x=0;x<w*2;x+=2){		    
        p.fillRect(x*pixsize,y*pixsize,pixsize*2,pixsize,egacolor[data[y*w*2+w*2-2-x]]);
      }
    }     
  }
  else{
    for(y=0;y<h;y++){
      for(x=0;x<w*2;x+=2){		
        p.fillRect(x*pixsize,y*pixsize,pixsize*2,pixsize,egacolor[data[y*w*2+x]]);
      }
    }
  }
  repaint();

}

//*****************************************
void PreviewView::draw(int ResNum)
{
  int err = view->open(ResNum);
  if(!err){
    preview->showlooppar();
    preview->showcelpar();
  }  
  update();
  if(!err && view->Description != ""){
    show_description();
    preview->description->show();
  }
  else{
    preview->description->hide();
  }

}

//*****************************************
void PreviewView::show_description()
{
  int x,y,w,h,W,H,n;
  unsigned int maxcol;

  preview->description->hide();
  w = (view->loops[view->CurLoop].cels[view->CurCel].width)*2*pixsize;
  h = (view->loops[view->CurLoop].cels[view->CurCel].height)*pixsize;
  resize(w,h);
  
  W=preview->width();
  H=preview->height();
  x=this->x();
  y=this->y();
  preview->description->setGeometry(10,y+h+10,W-20,H-(y+h+20));

  QFontMetrics f = fontMetrics();
  maxcol = (W-20)/f.width('a');

  preview->description->clear();
  string ThisLine = "";
  string ThisMessage = view->Description;

  do{
    if(ThisMessage.length() + ThisLine.length() > maxcol){
      n = maxcol - ThisLine.length();
      do{ n--; }while(!(n == 0 || ThisMessage[n]==' '));
      if (n <= 0)n = maxcol-ThisLine.length();
      ThisLine += ThisMessage.substr(0,n);      
      ThisMessage = (n < (int)ThisMessage.length())?ThisMessage.substr(n+1):"";
      preview->description->insertLine(ThisLine.c_str(),-1);
      ThisLine = "";
    }
    else{
      ThisLine += ThisMessage;
      ThisMessage = "";
    }
    
  }while(ThisMessage != "");     

  if(ThisLine != ""){
    preview->description->insertLine(ThisLine.c_str(),-1);
  }  

}
//*****************************************
