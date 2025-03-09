/*
 * cBufferViewer.h
 *
 *  Created on: 2012-06-04
 *      Author: dri
 *
 *  This is the C++ implementation of BufferViewer.py
 */

#ifndef CBUFFERVIEWER_H_
#define CBUFFERVIEWER_H_

#include <pandaFramework.h>
#include "cardMaker.h"
class CBufferViewer
   {
   public:

   enum CardPosition
      {
      CP_llcorner, // put the cards in the lower-left  corner of the window
      CP_lrcorner, // put the cards in the lower-right corner of the window
      CP_ulcorner, // put the cards in the upper-left  corner of the window
      CP_urcorner, // put the cards in the upper-right corner of the window
      CP_window    // put the cards in a separate window
      };

   enum CardLayout
      {
      CL_vline, // display the cards in a vertical line
      CL_hline, // display the cards in a horizontal line
      CL_vgrid, // display the cards in a vertical grid
      CL_hgrid, // display the cards in a horizontal grid
      CL_cycle  // display one card at a time, using selectCard/advanceCard
      };

   CBufferViewer(WindowFramework* windowFrameworkPtr);

   void refresh_readout();
   bool is_enabled();
   void enable(bool x);
   void toggle_enable();
   void set_card_size(float x, float y);
   void set_position(CardPosition pos);
   void set_layout(CardLayout lay);
   void set_include_all();
   void set_include(const pvector<Texture*>& x);
   void set_include(const pvector<GraphicsOutput*>& x);
   void set_exclude_none();
   void set_exclude(const pvector<Texture*>& x);
   void set_exclude(const pvector<GraphicsOutput*>& x);
   void set_sort(const std::string& bin, int sort);
   void set_render_parent(NodePath renderParent);

   private:

   void select_card(int i);
   void advance_card();
   AsyncTask::DoneStatus maintain_readout(GenericAsyncTask* taskPtr);
   NodePath make_frame(int sizex, int sizey);

   CBufferViewer(); // to prevent use of the default constructor
   void set_include(const GraphicsEngine* x);
   static AsyncTask::DoneStatus call_maintain_readout(GenericAsyncTask* taskPtr, void* dataPtr);
   static void call_refresh_readout(const Event* eventPtr, void* dataPtr);

   PT(WindowFramework) m_windowFrameworkPtr;
   bool m_enabled;
   float m_sizex;
   float m_sizey;
   CardPosition m_position;
   CardLayout m_layout;
   bool m_includeAll;
   pvector<Texture*> m_include;
   pvector<Texture*> m_exclude;
   std::string m_cullbin;
   int m_cullsort;
   NodePath m_renderParent;
   pvector<NodePath> m_cards;
   int m_cardindex;
   CardMaker m_cardmaker;
   PT(GenericAsyncTask) m_task;
   bool m_dirty;
   };

#endif /* CBUFFERVIEWER_H_ */
