// Copyright (c) 2014 INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
//
// Author(s)     : Clement Jamin

#ifndef CGAL_REGULAR_TRIANGULATION_H
#define CGAL_REGULAR_TRIANGULATION_H

#include <CGAL/Triangulation.h>
#include <CGAL/Dimension.h>
#include <CGAL/Default.h>

namespace CGAL {

template< typename RTTraits, typename TDS_ = Default >
class Regular_triangulation
: public Triangulation<
    RTTraits,
    typename Default::Get<TDS_, Triangulation_data_structure<
                          typename RTTraits::Dimension,
                          Triangulation_vertex<RTTraits>,
                          Triangulation_full_cell<RTTraits> >
                         >::type >
{
  typedef typename RTTraits::Dimension            Maximal_dimension_;
  typedef typename Default::Get<
    TDS_, 
    Triangulation_data_structure<
    Maximal_dimension_,
    Triangulation_vertex<RTTraits>,
    Triangulation_full_cell<RTTraits> 
    > >::type                                     TDS;
  typedef Triangulation<RTTraits, TDS>            Base;
  typedef Regular_triangulation<RTTraits, TDS_>   Self;
  
  typedef typename RTTraits::Orientation_d                 Orientation_d;
  typedef typename RTTraits::Construct_weighted_point_d    Construct_weighted_point_d;
  typedef typename RTTraits::Power_test_d                  Power_test_d;
  typedef typename RTTraits::In_flat_power_test_d          In_flat_power_test_d;
  typedef typename RTTraits::Flat_orientation_d            Flat_orientation_d;
  typedef typename RTTraits::Construct_flat_orientation_d  Construct_flat_orientation_d;
  typedef typename RTTraits::In_flat_orientation_d         In_flat_orientation_d;

public: // PUBLIC NESTED TYPES

  typedef RTTraits                                Geom_traits;
  typedef typename Base::Triangulation_ds         Triangulation_ds;

  typedef typename Base::Vertex                   Vertex;
  typedef typename Base::Full_cell                Full_cell;
  typedef typename Base::Facet                    Facet;
  typedef typename Base::Face                     Face;
  
  typedef Maximal_dimension_                      Maximal_dimension;
  typedef typename RTTraits::Bare_point           Bare_point;
  typedef typename RTTraits::Weighted_point       Weighted_point;

  typedef typename Base::Point_const_iterator     Point_const_iterator;
  typedef typename Base::Vertex_handle            Vertex_handle;
  typedef typename Base::Vertex_iterator          Vertex_iterator;
  typedef typename Base::Vertex_const_handle      Vertex_const_handle;
  typedef typename Base::Vertex_const_iterator    Vertex_const_iterator;

  typedef typename Base::Full_cell_handle         Full_cell_handle;
  typedef typename Base::Full_cell_iterator       Full_cell_iterator;
  typedef typename Base::Full_cell_const_handle   Full_cell_const_handle;
  typedef typename Base::Full_cell_const_iterator Full_cell_const_iterator;
  typedef typename Base::Finite_full_cell_const_iterator
                                                  Finite_full_cell_const_iterator;

  typedef typename Base::size_type                size_type;
  typedef typename Base::difference_type          difference_type;

  typedef typename Base::Locate_type              Locate_type;

  //Tag to distinguish Delaunay from Regular triangulations
  typedef Tag_true                                  Weighted_tag;

protected: // DATA MEMBERS


public:

  using typename Base::Rotor;
  using Base::maximal_dimension;
  using Base::are_incident_full_cells_valid;
  using Base::coaffine_orientation_predicate;
  using Base::reset_flat_orientation;
  using Base::current_dimension;
  using Base::geom_traits;
  using Base::index_of_covertex;
  using Base::index_of_second_covertex;
  using Base::rotate_rotor;
  using Base::infinite_vertex;
  using Base::insert_in_hole;
  using Base::insert_outside_convex_hull_1;
  using Base::is_infinite;
  using Base::locate;
  using Base::points_begin;
  using Base::set_neighbors;
  using Base::new_full_cell;
  using Base::number_of_vertices;
  using Base::orientation;
  using Base::tds;
  using Base::reorient_full_cells;
  using Base::full_cell;
  using Base::full_cells_begin;
  using Base::full_cells_end;
  using Base::finite_full_cells_begin;
  using Base::finite_full_cells_end;
  using Base::vertices_begin;
  using Base::vertices_end;

private:
  //*** Power_test_in_flat_d *** CJTODO: better name?
  // Wrapper
  struct Power_test_in_flat_d
  {
    boost::optional<Flat_orientation_d>* fop;
    Construct_flat_orientation_d cfo;
    In_flat_power_test_d ifpt;

    Power_test_in_flat_d(
      boost::optional<Flat_orientation_d>& x,
      Construct_flat_orientation_d const&y,
      In_flat_power_test_d const&z)
    : fop(&x), cfo(y), ifpt(z) {}

    template<class Iter>
    CGAL::Orientation operator()(Iter a, Iter b, const Weighted_point & p)const
    {
      if(!*fop)
        *fop=cfo(a,b);
      return ifpt(fop->get(),a,b,p);
    }
  };
public:
  
// - - - - - - - - - - - - - - - - - - - - - - - - - - CREATION / CONSTRUCTORS

  Regular_triangulation(int dim, const Geom_traits &k = Geom_traits())
  : Base(dim, k)
  {
  }

  // With this constructor,
  // the user can specify a Flat_orientation_d object to be used for 
  // orienting simplices of a specific dimension 
  // (= preset_flat_orientation_.first)
  // It it used by the dark triangulations created by DT::remove
  Regular_triangulation(
    int dim, 
    const std::pair<int, const Flat_orientation_d *> &preset_flat_orientation,
    const Geom_traits &k = Geom_traits())
  : Base(dim, preset_flat_orientation, k)
  {
  }

  ~Regular_triangulation() {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ACCESS

  // Not Documented
  Power_test_in_flat_d power_test_in_flat_predicate() const
  {
    return Power_test_in_flat_d (
      flat_orientation_, 
      geom_traits().construct_flat_orientation_d_object(), 
      geom_traits().in_flat_power_test_d_object()
    );
  }


  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - REMOVALS

  Full_cell_handle remove(Vertex_handle);
  Full_cell_handle remove(const Weighted_point & p, Full_cell_handle hint = Full_cell_handle())
  {
    Locate_type lt;
    Face f(maximal_dimension());
    Facet ft;
    Full_cell_handle s = locate(p, lt, f, ft, hint);
    if( Base::ON_VERTEX == lt )
    {
      return remove(s->vertex(f.index(0)));
    }
    return Full_cell_handle();
  }

  template< typename ForwardIterator >
  void remove(ForwardIterator start, ForwardIterator end)
  {
    while( start != end )
      remove(*start++);
  }

  // Not documented
  void remove_decrease_dimension(Vertex_handle);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - INSERTIONS

  template< typename ForwardIterator >
  size_type insert(ForwardIterator start, ForwardIterator end)
  {
    size_type n = number_of_vertices();
    typedef std::vector<Weighted_point> WP_vec;
    WP_vec points(start, end);
    typename Geom_traits::Point_drop_weight_d pdw =
      geom_traits().point_drop_weight_d_object();
    spatial_sort(
      boost::make_transform_iterator(points.begin(), pdw),
      boost::make_transform_iterator(points.end(), pdw),
      typename Geom_traits::Base());
    //spatial_sort(points.begin(), points.end(), geom_traits()); // CJTODO TEMP A REMETTRE
    //spatial_sort(points.begin(), points.end(), Geom_traits::Base());
    Full_cell_handle hint;
    for(typename WP_vec::const_iterator p = points.begin(); p != points.end(); ++p )
    {
      Locate_type lt;
      Face f(maximal_dimension());
      Facet ft;
      Full_cell_handle c = locate (*p, lt, f, ft, hint);
      Vertex_handle v = insert (*p, lt, f, ft, c);

      hint = v == Vertex_handle() ? c : v->full_cell();
    }
    return number_of_vertices() - n;
  }

  Vertex_handle insert(const Weighted_point &, 
                       const Locate_type, 
                       const Face &, 
                       const Facet &, 
                       const Full_cell_handle);

  Vertex_handle insert(const Weighted_point & p, 
                       const Full_cell_handle start = Full_cell_handle())
  {
    Locate_type lt;
    Face f(maximal_dimension());
    Facet ft;
    Full_cell_handle s = locate(p, lt, f, ft, start);
    return insert(p, lt, f, ft, s);
  }

  Vertex_handle insert(const Weighted_point & p, const Vertex_handle hint)
  {
    CGAL_assertion( Vertex_handle() != hint );
    return insert(p, hint->full_cell());
  }

  Vertex_handle insert_outside_affine_hull(const Weighted_point &);
  Vertex_handle insert_in_conflicting_cell(
    const Weighted_point &, const Full_cell_handle,
    const Vertex_handle only_if_this_vertex_is_in_the_cz = Vertex_handle());
  
  Vertex_handle insert_if_in_star(const Weighted_point &, 
                                  const Vertex_handle, 
                                  const Locate_type, 
                                  const Face &, 
                                  const Facet &, 
                                  const Full_cell_handle);
  
  Vertex_handle insert_if_in_star(
    const Weighted_point & p, const Vertex_handle star_center,
    const Full_cell_handle start = Full_cell_handle())
  {
    Locate_type lt;
    Face f(maximal_dimension());
    Facet ft;
    Full_cell_handle s = locate(p, lt, f, ft, start);
    return insert_if_in_star(p, star_center, lt, f, ft, s);
  }

  Vertex_handle insert_if_in_star(
    const Weighted_point & p, const Vertex_handle star_center, 
    const Vertex_handle hint)
  {
    CGAL_assertion( Vertex_handle() != hint );
    return insert_if_in_star(p, hint->full_cell());
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - GATHERING CONFLICTING SIMPLICES

  bool is_in_conflict(const Weighted_point &, Full_cell_const_handle) const;

  template< class OrientationPredicate >
  Oriented_side perturbed_power_test(const Weighted_point &,
      Full_cell_const_handle, const OrientationPredicate &) const;

  template< typename OutputIterator >
  Facet compute_conflict_zone(const Weighted_point &, const Full_cell_handle, OutputIterator) const;

  template < typename OrientationPredicate, typename PowerTestPredicate >
  class Conflict_predicate
  {
    const Self & rt_;
    const Weighted_point & p_;
    OrientationPredicate ori_;
    PowerTestPredicate power_test_;
    int cur_dim_;
  public:
    Conflict_predicate(
      const Self & rt,
      const Weighted_point & p,
      const OrientationPredicate & ori,
      const PowerTestPredicate & power_test)
    : rt_(rt), p_(p), ori_(ori), power_test_(power_test), cur_dim_(rt.current_dimension()) {}

    inline
    bool operator()(Full_cell_const_handle s) const
    {
      bool ok;
      if( ! rt_.is_infinite(s) )
      {
        Oriented_side power_test = power_test_(rt_.points_begin(s), rt_.points_begin(s) + cur_dim_ + 1, p_);
        if( ON_POSITIVE_SIDE == power_test )
          ok = true;
        else if( ON_NEGATIVE_SIDE == power_test )
          ok = false;
        else
          ok = ON_POSITIVE_SIDE == rt_.perturbed_power_test<OrientationPredicate>(p_, s, ori_);
      }
      else
      {
        typedef typename Full_cell::Vertex_handle_const_iterator VHCI;
        typedef Substitute_point_in_vertex_iterator<VHCI> F;
        F spivi(rt_.infinite_vertex(), &p_);

        Orientation o =  ori_(
          boost::make_transform_iterator(s->vertices_begin(), spivi),
          boost::make_transform_iterator(s->vertices_begin() + cur_dim_ + 1, 
                                         spivi));

        if( POSITIVE == o )
          ok = true;
        else if( o == NEGATIVE )
          ok = false;
        else
          ok = (*this)(s->neighbor( s->index( rt_.infinite_vertex() ) ));
      }
      return ok;
    }
  };

  template < typename ConflictPredicate >
  class Conflict_traversal_predicate
  {
    const Self & rt_;
    const ConflictPredicate & pred_;
  public:
    Conflict_traversal_predicate(const Self & rt, const ConflictPredicate & pred)
    : rt_(rt), pred_(pred)
    {}
    inline
    bool operator()(const Facet & f) const
    {
      return pred_(rt_.full_cell(f)->neighbor(rt_.index_of_covertex(f)));
    }
  };
  
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  VALIDITY
    
    bool is_valid(bool verbose = false, int level = 0) const;

private:
  
  template<typename InputIterator>
  bool
  does_cell_range_contain_vertex(InputIterator cz_begin, InputIterator cz_end, 
                                 Vertex_handle vh) const
  {
    // Check all vertices
    while(cz_begin != cz_end)
    {
      Full_cell_handle fch = *cz_begin;
      for (int i = 0 ; i <= current_dimension() ; ++i)
      {
        if (fch->vertex(i) == vh)
          return true;
      }
      ++cz_begin;
    }
    return false;
  }

  template<typename InputIterator, typename OutputIterator>
  void
  process_conflict_zone(InputIterator cz_begin, InputIterator cz_end, 
                        OutputIterator vertices_out) const
  {
    // Get all vertices
    while(cz_begin != cz_end)
    {
      Full_cell_handle fch = *cz_begin;
      for (int i = 0 ; i <= current_dimension() ; ++i)
      {
        Vertex_handle vh = fch->vertex(i);
        if (vh->full_cell() != Full_cell_handle())
        {
          (*vertices_out++) = vh;
          vh->set_full_cell(Full_cell_handle());
        }
      }
      ++cz_begin;
    }
  }

  
  template<typename InputIterator>
  void
  process_cz_vertices_after_insertion(InputIterator vertices_begin, 
                                      InputIterator vertices_end)
  {
    // Get all vertices
    while(vertices_begin != vertices_end)
    {
      Vertex_handle vh = *vertices_begin;
      if (vh->full_cell() == Full_cell_handle())
      {
        m_hidden_points.push_back(vh->point());
        tds().delete_vertex(vh);
      }
      ++vertices_begin;
    }
  }

private:
  // Some internal types to shorten notation
  using typename Base::Coaffine_orientation_d;
  using Base::flat_orientation_;
  typedef Conflict_predicate<Coaffine_orientation_d, Power_test_in_flat_d>
      Conflict_pred_in_subspace;
  typedef Conflict_predicate<Orientation_d, Power_test_d>
      Conflict_pred_in_fullspace;
  typedef Conflict_traversal_predicate<Conflict_pred_in_subspace>
      Conflict_traversal_pred_in_subspace;
  typedef Conflict_traversal_predicate<Conflict_pred_in_fullspace>
      Conflict_traversal_pred_in_fullspace;
   
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  MEMBER VARIABLES
  std::vector<Weighted_point> m_hidden_points;

}; // class Regular_triangulation


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
// FUNCTIONS THAT ARE MEMBER METHODS:

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - REMOVALS

template< typename RTTraits, typename TDS >
typename Regular_triangulation<RTTraits, TDS>::Full_cell_handle
Regular_triangulation<RTTraits, TDS>
::remove( Vertex_handle v )
{
  CGAL_precondition( ! is_infinite(v) );
  CGAL_expensive_precondition( is_vertex(v) );

  // THE CASE cur_dim == 0
  if( 0 == current_dimension() )
  {
    remove_decrease_dimension(v);
    return Full_cell_handle();
  }
  else if( 1 == current_dimension() )
  {   // THE CASE cur_dim == 1
    if( 2 == number_of_vertices() )
    {
      remove_decrease_dimension(v);
      return Full_cell_handle();
    }
    Full_cell_handle left = v->full_cell();
    if( is_infinite(left) && left->neighbor(0)->index(left) == 0 ) // we are on the infinite right.
      left = left->neighbor(0);
    if( 0 == left->index(v) )
      left = left->neighbor(1);
    CGAL_assertion( 1 == left->index(v) );
    Full_cell_handle right = left->neighbor(0);
    if( ! is_infinite(right) )
    {
      tds().associate_vertex_with_full_cell(left, 1, right->vertex(1));
      set_neighbors(left, 0, right->neighbor(0), right->mirror_index(0));
    }
    else
    {
      tds().associate_vertex_with_full_cell(left, 1, left->vertex(0));
      tds().associate_vertex_with_full_cell(left, 0, infinite_vertex());
      set_neighbors(left, 0, left->neighbor(1), left->mirror_index(1));
      set_neighbors(left, 1, right->neighbor(1), right->mirror_index(1));
    }
    tds().delete_vertex(v);
    tds().delete_full_cell(right);
    return left;
  }

  // THE CASE cur_dim >= 2
  // Gather the finite vertices sharing an edge with |v|
  typedef typename Base::template Full_cell_set<Full_cell_handle> Simplices;
  Simplices simps;
  std::back_insert_iterator<Simplices> out(simps);
  tds().incident_full_cells(v, out);
  typedef std::set<Vertex_handle> Vertex_set;
  Vertex_set verts;
  Vertex_handle vh;
  for( typename Simplices::iterator it = simps.begin(); it != simps.end(); ++it )
    for( int i = 0; i <= current_dimension(); ++i )
    {
      vh = (*it)->vertex(i);
      if( is_infinite(vh) )
        continue;
      if( vh == v )
        continue;
      verts.insert(vh);
    }

  // After gathering finite neighboring vertices, create their Dark Delaunay triangulation
  typedef Triangulation_vertex<Geom_traits, Vertex_handle> Dark_vertex_base;
  typedef Triangulation_full_cell<
    Geom_traits,
    internal::Triangulation::Dark_full_cell_data<Self> >   Dark_full_cell_base;
  typedef Triangulation_data_structure<Maximal_dimension, 
                                       Dark_vertex_base, 
                                       Dark_full_cell_base
                                      >                    Dark_tds;
  typedef Regular_triangulation<RTTraits, Dark_tds>        Dark_triangulation;
  typedef typename Dark_triangulation::Face                Dark_face;
  typedef typename Dark_triangulation::Facet               Dark_facet;
  typedef typename Dark_triangulation::Vertex_handle       Dark_v_handle;
  typedef typename Dark_triangulation::Full_cell_handle    Dark_s_handle;

  // If flat_orientation_ is defined, we give it the Dark triangulation
  // so that the orientation it uses for "current_dimension()"-simplices is
  // coherent with the global triangulation
  Dark_triangulation dark_side(
    maximal_dimension(),
    flat_orientation_ ?
    std::pair<int, const Flat_orientation_d *>(current_dimension(), flat_orientation_.get_ptr())
    : std::pair<int, const Flat_orientation_d *>(std::numeric_limits<int>::max(), NULL) );

  Dark_s_handle dark_s;
  Dark_v_handle dark_v;
  typedef std::map<Vertex_handle, Dark_v_handle> Vertex_map;
  Vertex_map light_to_dark;
  typename Vertex_set::iterator vit = verts.begin();
  while( vit != verts.end() )
  {
    dark_v = dark_side.insert((*vit)->point(), dark_s);
    dark_s = dark_v->full_cell();
    dark_v->data() = *vit;
    light_to_dark[*vit] = dark_v;
    ++vit;
  }

  if( dark_side.current_dimension() != current_dimension() )
  {
    CGAL_assertion( dark_side.current_dimension() + 1 == current_dimension() );
    // Here, the finite neighbors of |v| span a affine subspace of
    // dimension one less than the current dimension. Two cases are possible:
    if( (size_type)(verts.size() + 1) == number_of_vertices() )
    {
      remove_decrease_dimension(v);
      return Full_cell_handle();
    }
    else
    {   // |v| is strictly outside the convex hull of the rest of the points. This is an
      // easy case: first, modify the finite full_cells, then, delete the infinite ones.
      // We don't even need the Dark triangulation.
      Simplices infinite_simps;
      {
        Simplices finite_simps;
        for( typename Simplices::iterator it = simps.begin(); it != simps.end(); ++it )
          if( is_infinite(*it) )
            infinite_simps.push_back(*it);
          else
            finite_simps.push_back(*it);
        simps.swap(finite_simps);
      } // now, simps only contains finite simplices
      // First, modify the finite full_cells:
      for( typename Simplices::iterator it = simps.begin(); it != simps.end(); ++it )
      {
        int v_idx = (*it)->index(v);
        tds().associate_vertex_with_full_cell(*it, v_idx, infinite_vertex());
        if( v_idx != 0 )
        {
          // we must put the infinite vertex at index 0.
          // OK, now with the new convention that the infinite vertex
          // does not have to be at index 0, this is not necessary,
          // but still, I prefer to keep this piece of code here. [-- Samuel Hornus]
          (*it)->swap_vertices(0, v_idx);
          // Now, we preserve the positive orientation of the full_cell
          (*it)->swap_vertices(current_dimension() - 1, current_dimension());
        }
      }
      // Make the handles to infinite full cells searchable
      infinite_simps.make_searchable();
      // Then, modify the neighboring relation
      for( typename Simplices::iterator it = simps.begin(); it != simps.end(); ++it )
      {
        for( int i = 1; i <= current_dimension(); ++i )
        {
          (*it)->vertex(i)->set_full_cell(*it);
          Full_cell_handle n = (*it)->neighbor(i);
          // Was |n| a finite full cell prior to removing |v| ?
          if( ! infinite_simps.contains(n) )
            continue;
          int n_idx = n->index(v);
          set_neighbors(*it, i, n->neighbor(n_idx), n->neighbor(n_idx)->index(n));
        }
      }
      Full_cell_handle ret_s;
      // Then, we delete the infinite full_cells
      for( typename Simplices::iterator it = infinite_simps.begin(); it != infinite_simps.end(); ++it )
        tds().delete_full_cell(*it);
      tds().delete_vertex(v);
      return simps.front();
    }
  }
  else //  From here on, dark_side.current_dimension() == current_dimension()
  {
    dark_side.infinite_vertex()->data() = infinite_vertex();
    light_to_dark[infinite_vertex()] = dark_side.infinite_vertex();
  }

  // Now, compute the conflict zone of v->point() in
  // the dark side. This is precisely the set of full_cells
  // that we have to glue back into the light side.
  Dark_face       dark_f(dark_side.maximal_dimension());
  Dark_facet      dark_ft;
  typename Dark_triangulation::Locate_type     lt;
  dark_s = dark_side.locate(v->point(), lt, dark_f, dark_ft);
  CGAL_assertion( lt != Dark_triangulation::ON_VERTEX
    && lt != Dark_triangulation::OUTSIDE_AFFINE_HULL );

  // |ret_s| is the full_cell that we return
  Dark_s_handle dark_ret_s = dark_s;
  Full_cell_handle ret_s;

  typedef typename Base::template Full_cell_set<Dark_s_handle> Dark_full_cells;
  Dark_full_cells conflict_zone;
  std::back_insert_iterator<Dark_full_cells> dark_out(conflict_zone);
  
  dark_ft = dark_side.compute_conflict_zone(v->point(), dark_s, dark_out);
  // Make the dark simplices in the conflict zone searchable
  conflict_zone.make_searchable();

  // THE FOLLOWING SHOULD MAYBE GO IN TDS.
  // Here is the plan:
  // 1. Pick any Facet from boundary of the light zone
  // 2. Find corresponding Facet on boundary of dark zone
  // 3. stitch.

  // 1. Build a facet on the boudary of the light zone:
  Full_cell_handle light_s = *simps.begin();
  Facet light_ft(light_s, light_s->index(v));

  // 2. Find corresponding Dark_facet on boundary of the dark zone
  Dark_full_cells dark_incident_s;
  for( int i = 0; i <= current_dimension(); ++i )
  {
    if( index_of_covertex(light_ft) == i )
      continue;
    Dark_v_handle dark_v = light_to_dark[full_cell(light_ft)->vertex(i)];
    dark_incident_s.clear();
    dark_out = std::back_inserter(dark_incident_s);
    dark_side.tds().incident_full_cells(dark_v, dark_out);
    for(typename Dark_full_cells::iterator it = dark_incident_s.begin(); 
        it != dark_incident_s.end(); 
        ++it)
    {
      (*it)->data().count_ += 1;
    }
  }

  for( typename Dark_full_cells::iterator it = dark_incident_s.begin(); it != dark_incident_s.end(); ++it )
  {
    if( current_dimension() != (*it)->data().count_ )
      continue;
    if( ! conflict_zone.contains(*it) )
      continue;
    // We found a full_cell incident to the dark facet corresponding to the light facet |light_ft|
    int ft_idx = 0;
    while( light_s->has_vertex( (*it)->vertex(ft_idx)->data() ) )
      ++ft_idx;
    dark_ft = Dark_facet(*it, ft_idx);
    break;
  }
  // Pre-3. Now, we are ready to traverse both boundary and do the stiching.

  // But first, we create the new full_cells in the light triangulation,
  // with as much adjacency information as possible.

  // Create new full_cells with vertices
  for( typename Dark_full_cells::iterator it = conflict_zone.begin(); it != conflict_zone.end(); ++it )
  {
    Full_cell_handle new_s = new_full_cell();
    (*it)->data().light_copy_ = new_s;
    for( int i = 0; i <= current_dimension(); ++i )
      tds().associate_vertex_with_full_cell(new_s, i, (*it)->vertex(i)->data());
    if( dark_ret_s == *it )
      ret_s = new_s;
  }

  // Setup adjacencies inside the hole
  for( typename Dark_full_cells::iterator it = conflict_zone.begin(); it != conflict_zone.end(); ++it )
  {
    Full_cell_handle new_s = (*it)->data().light_copy_;
    for( int i = 0; i <= current_dimension(); ++i )
      if( conflict_zone.contains((*it)->neighbor(i)) )
        tds().set_neighbors(new_s, i, (*it)->neighbor(i)->data().light_copy_, (*it)->mirror_index(i));
  }

  // 3. Stitch
  simps.make_searchable();
  typedef std::queue<std::pair<Facet, Dark_facet> > Queue;
  Queue q;
  q.push(std::make_pair(light_ft, dark_ft));
  dark_s = dark_side.full_cell(dark_ft);
  int dark_i = dark_side.index_of_covertex(dark_ft);
  // mark dark_ft as visited:
  // TODO try by marking with Dark_v_handle (vertex)
  dark_s->neighbor(dark_i)->set_neighbor(dark_s->mirror_index(dark_i), Dark_s_handle());
  while( ! q.empty() )
  {
    std::pair<Facet, Dark_facet> p = q.front();
    q.pop();
    light_ft = p.first;
    dark_ft = p.second;
    light_s = full_cell(light_ft);
    int light_i = index_of_covertex(light_ft);
    dark_s = dark_side.full_cell(dark_ft);
    int dark_i = dark_side.index_of_covertex(dark_ft);
    Full_cell_handle light_n = light_s->neighbor(light_i);
    set_neighbors(dark_s->data().light_copy_, dark_i, light_n, light_s->mirror_index(light_i));
    for( int di = 0; di <= current_dimension(); ++di )
    {
      if( di == dark_i )
        continue;
      int li = light_s->index(dark_s->vertex(di)->data());
      Rotor light_r(light_s, li, light_i);
      typename Dark_triangulation::Rotor dark_r(dark_s, di, dark_i);
      
      while( simps.contains(full_cell(light_r)->neighbor(index_of_covertex(light_r))) )
        light_r = rotate_rotor(light_r);

      while( conflict_zone.contains(dark_side.full_cell(dark_r)->neighbor(dark_side.index_of_covertex(dark_r))) )
        dark_r = dark_side.rotate_rotor(dark_r);

      Dark_s_handle dark_ns = dark_side.full_cell(dark_r);
      int dark_ni = dark_side.index_of_covertex(dark_r);
      Full_cell_handle light_ns = full_cell(light_r);
      int light_ni = index_of_covertex(light_r);
      // mark dark_r as visited:
      // TODO try by marking with Dark_v_handle (vertex)
      Dark_s_handle outside = dark_ns->neighbor(dark_ni);
      Dark_v_handle mirror = dark_ns->mirror_vertex(dark_ni, current_dimension());
      int dn = outside->index(mirror);
      if( Dark_s_handle() == outside->neighbor(dn) )
        continue;
      outside->set_neighbor(dn, Dark_s_handle());
      q.push(std::make_pair(Facet(light_ns, light_ni), Dark_facet(dark_ns, dark_ni)));
    }
  }
  tds().delete_full_cells(simps.begin(), simps.end());
  tds().delete_vertex(v);
  return ret_s;
}

template< typename RTTraits, typename TDS >
void
Regular_triangulation<RTTraits, TDS>
::remove_decrease_dimension(Vertex_handle v)
{
  CGAL_precondition( current_dimension() >= 0 );
  tds().remove_decrease_dimension(v, infinite_vertex());
  // reset the predicates:
  reset_flat_orientation();
  if( 1 <= current_dimension() )
  {
    // FIXME: infinite vertex is NOT at index 0 a priori.
    Full_cell_handle s = infinite_vertex()->full_cell()->neighbor(0);
    Orientation o = orientation(s);
    CGAL_assertion( ZERO != o );
    if( NEGATIVE == o )
      reorient_full_cells();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - INSERTIONS

template< typename RTTraits, typename TDS >
typename Regular_triangulation<RTTraits, TDS>::Vertex_handle
Regular_triangulation<RTTraits, TDS>
::insert(const Weighted_point & p, const Locate_type lt, const Face & f, const Facet & ft, const Full_cell_handle s)
{
  switch( lt )
  {
    case Base::OUTSIDE_AFFINE_HULL:
      return insert_outside_affine_hull(p);
      break;
    case Base::ON_VERTEX:
    {
      Vertex_handle v = s->vertex(f.index(0));
      v->set_point(p);
      return v;
      break;
    }
    default:
      if( 1 == current_dimension() )
      {
        if( Base::OUTSIDE_CONVEX_HULL == lt )
        {
          return insert_outside_convex_hull_1(p, s);
        }
        Vertex_handle v = tds().insert_in_full_cell(s);
        v->set_point(p);
        return v;
      }
      else
        return insert_in_conflicting_cell(p, s);
      break;
  }
}

template< typename RTTraits, typename TDS >
typename Regular_triangulation<RTTraits, TDS>::Vertex_handle
Regular_triangulation<RTTraits, TDS>
::insert_outside_affine_hull(const Weighted_point & p)
{
  // we don't use Base::insert_outside_affine_hull(...) because here, we
  // also need to reset the side_of_oriented_subsphere functor.
  CGAL_precondition( current_dimension() < maximal_dimension() );
  Vertex_handle v = tds().insert_increase_dimension(infinite_vertex());
  // reset the predicates:
  reset_flat_orientation();
  v->set_point(p);
  if( current_dimension() >= 1 )
  {
    // FIXME: infinite vertex is NOT at index 0 a priori.
    Full_cell_handle s = infinite_vertex()->full_cell()->neighbor(0);
    Orientation o = orientation(s);
    CGAL_assertion( ZERO != o );
      if( NEGATIVE == o )
        reorient_full_cells();
  }
  return v;
}

template< typename RTTraits, typename TDS >
typename Regular_triangulation<RTTraits, TDS>::Vertex_handle
Regular_triangulation<RTTraits, TDS>
::insert_if_in_star(const Weighted_point & p, 
                    const Vertex_handle star_center,
                    const Locate_type lt, const Face & f, const Facet & ft, 
                    const Full_cell_handle s)
{
  switch( lt )
  {
    case Base::OUTSIDE_AFFINE_HULL:
      return insert_outside_affine_hull(p);
      break;
    case Base::ON_VERTEX:
    {
      Vertex_handle v = s->vertex(f.index(0));
      v->set_point(p);
      return v;
      break;
    }
    default:
    {
      if( 1 == current_dimension() )
      {
        if (s->has_vertex(star_center))
        {
          if( Base::OUTSIDE_CONVEX_HULL == lt )
          {
            return insert_outside_convex_hull_1(p, s);
          }
          Vertex_handle v = tds().insert_in_full_cell(s);
          v->set_point(p);
          return v;
        }
      }
      else
        return insert_in_conflicting_cell(p, s, star_center);
    break;
    }
  }

  return Vertex_handle();
}

template< typename RTTraits, typename TDS >
typename Regular_triangulation<RTTraits, TDS>::Vertex_handle
Regular_triangulation<RTTraits, TDS>
::insert_in_conflicting_cell(const Weighted_point & p, 
                             const Full_cell_handle s,
                             const Vertex_handle only_if_this_vertex_is_in_the_cz)
{
  typedef std::vector<Full_cell_handle> Full_cell_h_vector;

  Orientation_d ori = geom_traits().orientation_d_object();
  Power_test_d side = geom_traits().power_test_d_object();
  Conflict_pred_in_fullspace c(*this, p, ori, side);
  // If p is not in conflict with s, then p is hidden
  // => we don't insert it
  if (!c(s))
  {
    m_hidden_points.push_back(p);
    return Vertex_handle();
  }
  else
  {
    Full_cell_h_vector cs; // for storing conflicting full_cells.
    cs.reserve(64);
    std::back_insert_iterator<Full_cell_h_vector> out(cs);
    Facet ft = compute_conflict_zone(p, s, out);
    
    // Check if the CZ contains "only_if_this_vertex_is_in_the_cz"
    if (only_if_this_vertex_is_in_the_cz != Vertex_handle()
     && !does_cell_range_contain_vertex(cs.begin(), cs.end(), 
                                        only_if_this_vertex_is_in_the_cz))
    {
      return Vertex_handle();
    }

    // Otherwise, proceed with the insertion
    std::vector<Vertex_handle> cz_vertices;
    cz_vertices.reserve(64);
    process_conflict_zone(cs.begin(), cs.end(), 
                          std::back_inserter(cz_vertices));

    Vertex_handle ret = insert_in_hole(p, cs.begin(), cs.end(), ft);

    process_cz_vertices_after_insertion(cz_vertices.begin(), cz_vertices.end());

    return ret;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - GATHERING CONFLICTING SIMPLICES

// NOT DOCUMENTED
template< typename RTTraits, typename TDS >
template< typename OrientationPred >
Oriented_side
Regular_triangulation<RTTraits, TDS>
::perturbed_power_test(const Weighted_point & p, Full_cell_const_handle s,
    const OrientationPred & ori) const
{
  CGAL_precondition_msg( ! is_infinite(s), "full cell must be finite");
  CGAL_expensive_precondition( POSITIVE == orientation(s) );
  typedef std::vector<const Weighted_point *> Points;
  Points points(current_dimension() + 2);
  int i(0);
  for( ; i <= current_dimension(); ++i )
    points[i] = &(s->vertex(i)->point());
  points[i] = &p;
  std::sort(points.begin(), points.end(),
      internal::Triangulation::Compare_points_for_perturbation<Self>(*this));
  typename Points::const_reverse_iterator cut_pt = points.rbegin();
  Points test_points;
  while( cut_pt != points.rend() )
  {
    if( &p == *cut_pt )
      // because the full_cell "s" is assumed to be positively oriented
      return ON_NEGATIVE_SIDE; // we consider |p| to lie outside the sphere
    test_points.clear();
    Point_const_iterator spit = points_begin(s);
    int adjust_sign = -1;
    for( i = 0; i < current_dimension(); ++i )
    {
      if( &(*spit) == *cut_pt )
      {
        ++spit;
        adjust_sign = (((current_dimension() + i) % 2) == 0) ? -1 : +1;
      }
      test_points.push_back(&(*spit));
      ++spit;
    }
    test_points.push_back(&p);

    typedef typename CGAL::Iterator_project<
      typename Points::iterator,
      internal::Triangulation::Point_from_pointer<Self>,
      const Weighted_point &, const Weighted_point *
    > Point_pointer_iterator;

    Orientation ori_value = ori(
        Point_pointer_iterator(test_points.begin()),
        Point_pointer_iterator(test_points.end()));

    if( ZERO != ori_value )
      return Oriented_side( - adjust_sign * ori_value );

    ++cut_pt;
  }
  CGAL_assertion(false); // we should never reach here
  return ON_NEGATIVE_SIDE;
}

template< typename RTTraits, typename TDS >
bool
Regular_triangulation<RTTraits, TDS>
::is_in_conflict(const Weighted_point & p, Full_cell_const_handle s) const
{
  CGAL_precondition( 2 <= current_dimension() );
  if( current_dimension() < maximal_dimension() )
  {
    Conflict_pred_in_subspace c(
      *this, p, 
      coaffine_orientation_predicate(), 
      power_test_in_flat_predicate());
    return c(s);
  }
  else
  {
    Orientation_d ori = geom_traits().orientation_d_object(); // CJTODO: create member variables for this?
    Power_test_d side = geom_traits().power_test_d_object();
    Conflict_pred_in_fullspace c(*this, p, ori, side);
    return c(s);
  }
}

template< typename RTTraits, typename TDS >
template< typename OutputIterator >
typename Regular_triangulation<RTTraits, TDS>::Facet
Regular_triangulation<RTTraits, TDS>
::compute_conflict_zone(const Weighted_point & p, const Full_cell_handle s, OutputIterator out) const
{
  CGAL_precondition( 2 <= current_dimension() );
  if( current_dimension() < maximal_dimension() )
  {
    Conflict_pred_in_subspace c(
      *this, p, 
      coaffine_orientation_predicate(), 
      power_test_in_flat_predicate());
    Conflict_traversal_pred_in_subspace tp(*this, c);
    return tds().gather_full_cells(s, tp, out);
  }
  else
  {
    Orientation_d ori = geom_traits().orientation_d_object();
    Power_test_d side = geom_traits().power_test_d_object();
    Conflict_pred_in_fullspace c(*this, p, ori, side);
    Conflict_traversal_pred_in_fullspace tp(*this, c);
    return tds().gather_full_cells(s, tp, out);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - VALIDITY

template< typename RTTraits, typename TDS >
bool
Regular_triangulation<RTTraits, TDS>
::is_valid(bool verbose, int level) const
{ 
  if (!Base::is_valid(verbose, level))
    return false;

  int dim = current_dimension();
  if (dim == maximal_dimension())
  {
    for (Finite_full_cell_const_iterator cit = finite_full_cells_begin() ;
         cit != finite_full_cells_end() ; ++cit )
    {
      Full_cell_const_handle ch = cit.base();
      for(int i = 0; i < dim+1 ; ++i ) 
      {
        // If the i-th neighbor is not an infinite cell
        Vertex_handle opposite_vh = 
          ch->neighbor(i)->vertex(ch->neighbor(i)->index(ch));
        if (!is_infinite(opposite_vh))
        {
          Power_test_d side = 
            geom_traits().power_test_d_object();
          if (side(Point_const_iterator(ch->vertices_begin()), 
                   Point_const_iterator(ch->vertices_end()),
                   opposite_vh->point()) == ON_POSITIVE_SIDE)
          {
            if (verbose)
              CGAL_warning_msg(false, "Non-empty sphere");
            return false;
          }
        }
      }
    }
  }
  return true;
}

} //namespace CGAL

#endif //CGAL_REGULAR_TRIANGULATION_H
