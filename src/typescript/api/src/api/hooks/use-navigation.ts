import { useContext } from "react";
import { getGlobal } from "../globals";

/**
 * A React hook that lets you access methods to push and pop views
 * on the navigation stack.
 *
 * @category Navigation
 */
export const useNavigation = () => {
	const { push, pop } = useContext(getGlobal().navigationContext);

	return { push, pop };
};
