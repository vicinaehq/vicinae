import React, { lazy } from 'react';
import { Action, ActionPanel, Detail, showToast } from '@vicinae/api';

const md = `# Hello world

Your extension is *working* successfully.

Now you can start making changes to this command source file and see it update live here.

If you are online, you should be able to see the Vicinae logo below:

![](https://github.com/vicinaehq/vicinae/raw/main/.github/assets/vicinae-banner.png)
`;

export default function SimpleDetail() {
	return (
		<Detail 
			markdown={md} 
			actions={
				<ActionPanel>
					<Action title="Say hello" onAction={() => showToast({ title: 'Hello!' })} />
				</ActionPanel>
			}
		/>
	);
}
