/**
 * @file boundary_box_constraint.hpp
 * @author Suvarsha Chennareddy
 *
 * Boundary Box Transformation.
 *
 *
 * ensmallen is free software; you may redistribute it and/or modify it under
 * the terms of the 3-clause BSD license.  You should have received a copy of
 * the 3-clause BSD license along with ensmallen.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */

namespace ens {

/**
 * More often than not, coordinates must be bounded by some constraints.
 * In a particular case, the domain of a specific function is restricted 
 * by boundaries.
 * The implemented transformation transforms given coordinates into a region 
 * bounded by the given lower and upper bounds (a box). First, the 
 * coordinates are shifted into a feasible preimage bounded by lowerBound - al 
 * and upperBound + au where al and au and calculated internally. 
 * These shifted coordinates are then transformed into coordinates bounded by 
 * lower_bound and upper_bound. It is an identity transformation in between 
 * the lower and upper bounds.
 * The inverse transformation transforms the coordinates back to the preimage.
 * 
 * For more information, check the original implementation in C by N. Hansen:
 * https://github.com/CMA-ES/c-cmaes/blob/master/src/boundary_transformation.c
 *
 * @tparam MatType The matrix type of the coordinates and bounds.
 */
template<typename MatType = arma::mat>
class BoundaryBoxConstraint
{
public:

  /**
   * Construct the boundary box constraint policy.
   */
  BoundaryBoxConstraint()
  { /* Nothing to do. */ }

  /**
   * Construct the boundary box constraint policy.
   *
   * @param lowerBound The lower bound of the coordinates.
   * @param upperBound The upper bound of the coordinates.
   */
  BoundaryBoxConstraint(const MatType& lowerBound,
                        const MatType& upperBound) :
      lowerBound(lowerBound),
      upperBound(upperBound)
  {

  }
  
  /**
   * Construct the boundary box constraint policy.
   *
   * @param lowerBound The lower bound (for every dimension) of the coordinates.
   * @param upperBound The upper bound (for every dimension) of the coordinates.
   */
  BoundaryBoxConstraint(const double lowerBound,
                        const double upperBound) :
      lowerBound({ (typename MatType::elem_type) lowerBound }),
      upperBound({ (typename MatType::elem_type) upperBound })
  {}

  /**
   * Map the given coordinates to the range 
   * [lowerBound, upperBound]
   *
   * @param x Given coordinates.
   * @return Transformed coordinates.
   */
  MatType Transform(const MatType& x)
  {
    typedef typename MatType::elem_type ElemType;
    double diff, al, au, xlow, xup, r;
    size_t Bi, Bj;
    MatType y = x;
    for (size_t i = 0; i < x.n_rows; i++)
    {
      Bi = (i < lowerBound.n_rows) ? i : (lowerBound.n_rows - 1);
      for (size_t j = 0; j < x.n_cols; j++)
      {
        Bj = (j < lowerBound.n_cols) ? j : (lowerBound.n_cols - 1);

        diff = (upperBound(Bi, Bj) - lowerBound(Bi, Bj)) / 2.0;
        al = std::min(diff, (1 + std::abs(lowerBound(Bi, Bj))) / 20.0);
        au = std::min(diff, (1 + std::abs(upperBound(Bi, Bj))) / 20.0);
        xlow = lowerBound(Bi, Bj) - 2 * al - diff;
        xup = upperBound(Bi, Bj) + 2 * au + diff;
        r = 2 * (2 * diff + al + au);

        // Shift y into feasible pre-image.
        if (y(i, j) < xlow)
        {
          y(i,j) += (ElemType)(r * (1 + (int)(xlow - y(i, j)) / r));
        }
        if (y(i, j) > xup)
        {
          y(i, j) -= (ElemType)(r * (1 + (int)(y(i, j) - xup) / r));
        }
        if (y(i, j) < lowerBound(Bi, Bj) - al)
        {
          y(i, j) += (ElemType)(2 * (lowerBound(Bi, Bj) - al - y(i, j)));
        }
        if (y(i, j) > upperBound(Bi, Bj) + au)
        {
          y(i, j) -= (ElemType)(2 * (y(i, j) - upperBound(Bi, Bj) - au));
        }

        // Boundary transformation.
        if (y(i, j) < lowerBound(Bi, Bj) + al)
        {
          y(i, j) = (ElemType)(lowerBound(Bi, Bj) +
            (y(i, j) - (lowerBound(Bi, Bj) - al)) *
            (y(i, j) - (lowerBound(Bi, Bj) - al)) / 4.0 / al);
        }
        else if (y(i,j) > upperBound(Bi,Bj) - au)
        {
          y(i, j) = (ElemType)(upperBound(Bi, Bj) -
            (y(i, j) - (upperBound(Bi, Bj) + au)) *
            (y(i, j) - (upperBound(Bi, Bj) + au)) / 4.0 / au);
        }
      }
    }

    return y;
  }

  /**
   * Return a suitable initial step size.
   *
   * @return initial step size.
   */
  typename MatType::elem_type initialStepSize() {
    return 0.3*arma::min(arma::min(upperBound - lowerBound));
  }

  /**
   * Map the given coordinates back to the preimage, 
   * which is bounded by [lowerBound - al, upperBound + au].
   *
   * @param y Given coordinates.
   * @return Transformed coordinates.

  MatType Inverse(const MatType& y)
  {
    typedef typename MatType::elem_type ElemType;
    double diff, al, au;
    size_t Bi, Bj;
    MatType x = y;
    for (size_t i = 0; i < y.n_rows; i++)
    {
      Bi = (i < lowerBound.n_rows) ? i : (lowerBound.n_rows - 1);
      for (size_t j = 0; j < y.n_cols; j++)
      {
        Bj = (j < lowerBound.n_cols) ? j : (lowerBound.n_cols - 1);

        diff = (upperBound(Bi, Bj) - lowerBound(Bi, Bj)) / 2.0;
        al = std::min(diff, (1 + std::abs(lowerBound(Bi, Bj))) / 20.0);
        au = std::min(diff, (1 + std::abs(upperBound(Bi, Bj))) / 20.0);
        if (x(i, j) < lowerBound(Bi, Bj) + al)
        {
          x(i, j) = (ElemType)(lowerBound(Bi, Bj) - al) + 2 *
            std::sqrt(std::abs(al * (x(i, j) - lowerBound(Bi, Bj))));
        }
        else if (x(i, j) > upperBound(Bi, Bj) - au)
        {
          x(i, j) = (ElemType)(upperBound(Bi, Bj) + au) - 2 *
            std::sqrt(std::abs(au * (upperBound(Bi, Bj) - x(i, j))));
        }
      }
    }

    return x;
  }
  */

  //! Get the lower bound of decision variables.
  MatType LowerBound() const { return lowerBound; }
  //! Modify the lower bound of decision variables.
  MatType& LowerBound() { return lowerBound; }

  //! Get the upper bound of decision variables.
  MatType UpperBound() const { return upperBound; }
  //! Modify the upper bound of decision variables.
  MatType& UpperBound() { return upperBound; }

private:
  //! Upper bound of decision variables.
  MatType upperBound;

  //! Lower bound of decision variables.
  MatType lowerBound;
};

} // namespace ens
