import { useEffect, useState } from "react"
import { bus } from '../bus';

export type AppInfo = {
	id: string;
	name: string;
	icon: { iconName: string };
};

export const useApplications = (): [AppInfo[], boolean] => {
	const [isLoading, setIsLoading] = useState(true);
	const [apps, setApps] = useState<AppInfo[]>([]);

	useEffect(() => {
		console.log('get apps');
		bus!.request('list-applications', {})
		.then(({ data }) => {
			'got apps'
			setApps((data.apps as any[]).map<AppInfo>((app: any) => ({
				id: app.id,
				name: app.name,
				icon: { iconName: app.icon }
			})));
			setIsLoading(false);
		})
		.catch((error) => { console.error(`Failed to get apps`, error); })
	}, []);

	return [apps, isLoading];
} 
