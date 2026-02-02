import { useContext } from "react";
import navigationContext from "../context/navigation-context";

/**
 * A React hook that lets you access methods to push and pop views
 * on the navigation stack.
 *
 * @category Navigation
 */
export const useNavigation = () => {
	const { push, pop } = useContext(navigationContext);

	return { push, pop };
};
