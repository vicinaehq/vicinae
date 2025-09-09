import { PathLike } from "fs";
import { rm } from "fs/promises";
import { bus } from "./bus";
import { WindowManagement } from "./window-management";
import { Application } from "./proto/application";

// Linux systems usually do not have a trash, but maybe we should support one...
export const trash = async (path: PathLike | PathLike[]): Promise<void> => {
  const targets = Array.isArray(path) ? path : [path];
  const promises = targets.map((p) => rm(p, { recursive: true }));

  await Promise.all(promises);
};

export const open = async (target: string, app?: Application | string) => {
  let appId: string | undefined;

  if (app) {
    if (typeof app === "string") {
      appId = app;
    } else {
      appId = app.id;
    }
  }

  await bus.turboRequest("app.open", {
    target,
    appId,
  });
};

export const getFrontmostApplication = async (): Promise<Application> => {
  const { application } = await WindowManagement.getActiveWindow();

  if (!application) {
	throw new Error(`Could not get frontmost application`);
  }

  return application;
};

export const getApplications = async (
  path?: PathLike,
): Promise<Application[]> => {
  const res = await bus.turboRequest("app.list", {});

  return res.unwrap().apps;
};

export const getDefaultApplication = async (
  path: PathLike,
): Promise<Application> => {
	throw new Error("not implemented");
};

export const showInFileBrowser = async (path: PathLike): Promise<void> => {
	// TODO: get default app for file browser
	// open using it
};

export { Application } from "./proto/application";
