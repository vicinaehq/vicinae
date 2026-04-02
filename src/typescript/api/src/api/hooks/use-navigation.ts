import { useContext } from "react";
import { getNavigationContext } from "../context/navigation-context";

/**
 * A React hook that lets you access methods to push and pop views
 * on the navigation stack.
 *
 * @category Navigation
 */
export const useNavigation = () => {
	const { push, pop } = useContext(getNavigationContext());

	return { push, pop };
};
