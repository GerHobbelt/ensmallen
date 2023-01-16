/**
 * @file empty_transformation.hpp
 * @author Suvarsha Chennareddy
 *
 * Empty Transformation, can also be called an Indentity Transformation.
 *
 * ensmallen is free software; you may redistribute it and/or modify it under
 * the terms of the 3-clause BSD license.  You should have received a copy of
 * the 3-clause BSD license along with ensmallen.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */

namespace ens {

/**
 * This is an empty transformation. As the name indicates, it does
 * not do anything. It is essentially an identity 
 * transformation and is meant to be used when there are no
 * sorts of constraints on the coordinates.
 *
 * @tparam MatType The matrix type of the coordinates.
 */
template<typename MatType = arma::mat>
class EmptyTransformation
{
public:

	/**
	 * Transforms coordinates to themselves (effectively no transformation).
	 *
	 * @param x Input coordinates.
	 * @return Transformed coordinates (the coordinates themselves).
	 */
	MatType Transform(const MatType& x) { return x; }

	/**
	 * Return a suitable initial step size.
	 *
	 * @return initial step size.
	 */
	typename MatType::elem_type initialStepSize() { return 1; }

	/**
	 * Inverse transformtion (effectively no transformation).
	 *
	 * @param y Input coordinates.
	 * @return Transformed coordinates (the coordinates themselves).

	MatType Inverse(const MatType& y) { return y; }
	*/
};

} // namespace ens
