import { useContext } from "react";
import navigationContext from "../context/navigation-context";

export const useNavigation = () => {
	const { push, pop } = useContext(navigationContext);

	return { push, pop };
};
