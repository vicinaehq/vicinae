import { createHash, randomBytes, randomUUID } from "node:crypto";
import { Image, serializeProtoImage } from "./image";
import { bus } from "./bus";

enum OauthRedirectMethod {
	/**
	 * Use this type for a redirect back to the Raycast website, which will then open the extension.
	 * In the OAuth app, configure `https://raycast.com/redirect?packageName=Extension`
	 * (This is a static redirect URL for all extensions.)
	 * If the provider does not accept query parameters in redirect URLs, you can alternatively use `https://raycast.com/redirect/extension`
	 * and then customize the {@link OAuth.AuthorizationRequest} via its `extraParameters` property. For example add:
	 *  `extraParameters: { "redirect_uri": "https://raycast.com/redirect/extension" }`
	 */
	Web = "web",
	/**
	 * Use this type for an app-scheme based redirect that directly opens Raycast.
	 * In the OAuth app, configure `raycast://oauth?package_name=Extension`
	 */
	App = "app",
	/**
	 * Use this type for a URI-style app scheme that directly opens Raycast.
	 * In the OAuth app, configure `com.raycast:/oauth?package_name=Extension`
	 * (Note the single slash - Google, for example, would require this flavor for an OAuth app where the Bundle ID is `com.raycast`)
	 */
	AppURI = "appURI",
}

export declare namespace OAuth {
	export namespace PKCEClient {
		/**
		 * The options for creating a new {@link OAuth.PKCEClient}.
		 */
		export interface Options {
			/**
			 * The redirect method for the OAuth flow.
			 * Make sure to set this to the correct method for the provider, see {@link OAuth.RedirectMethod} for more information.
			 */
			redirectMethod: RedirectMethod;
			/**
			 * The name of the provider, displayed in the OAuth overlay.
			 */
			providerName: string;
			/**
			 * An icon displayed in the OAuth overlay.
			 * Make sure to provide at least a size of 64x64 pixels.
			 */
			providerIcon?: Image.ImageLike;
			/**
			 * An optional ID for associating the client with a provider.
			 * Only set this if you use multiple different clients in your extension.
			 */
			providerId?: string;
			/**
			 * An optional description, shown in the OAuth overlay.
			 * You can use this to customize the message for the end user, for example for handling scope changes or other migrations.
			 * Raycast shows a default message if this is not configured.
			 */
			description?: string;
		}
	}

	type RedirectMethod = OauthRedirectMethod;

	/**
	 * A client for the [OAuth PKCE extension](https://datatracker.ietf.org/doc/html/rfc7636).
	 *
	 * @example
	 * ```typescript
	 * import { OAuth } from "@raycast/api";
	 *
	 * const client = new OAuth.PKCEClient({
	 *   redirectMethod: OAuth.RedirectMethod.Web,
	 *   providerName: "Twitter",
	 *   providerIcon: "twitter-logo.png",
	 *   description: "Connect your Twitter account…",
	 * });
	 * ```
	 */

	/**
	 * Defines the supported redirect methods for the OAuth flow.
	 * You can choose between web and app-scheme redirect methods, depending on what the provider requires when setting up the OAuth app.
	 * For examples on what redirect URI you need to configure, see the docs for each method.
	 */

	/**
	 * The options for an authorization request via {@link OAuth.PKCEClient.authorizationRequest}.
	 */
	export interface AuthorizationRequestOptions {
		/**
		 * The URL to the authorization endpoint for the OAuth provider.
		 */
		endpoint: string;
		/**
		 * The client ID of the configured OAuth app.
		 */
		clientId: string;
		/**
		 * A space-delimited list of scopes for identifying the resources to access on the user's behalf.
		 * The scopes are typically shown to the user on the provider's consent screen in the browser.
		 * Note that some providers require the same scopes be configured in the registered OAuth app.
		 */
		scope: string;
		/**
		 * Optional additional parameters for the authorization request.
		 * Note that some providers require additional parameters, for example to obtain long-lived refresh tokens.
		 */
		extraParameters?: Record<string, string>;
	}
	/**
	 * Values of {@link OAuth.AuthorizationRequest}.
	 * The PKCE client automatically generates the values for you and returns them for {@link OAuth.PKCEClient.authorizationRequest}.
	 */
	export interface AuthorizationRequestURLParams {
		/**
		 * The PKCE `code_challenge` value.
		 */
		codeChallenge: string;
		/**
		 * The PKCE `code_verifier` value.
		 */
		codeVerifier: string;
		/**
		 * The OAuth `state` value.
		 */
		state: string;
		/**
		 * The OAuth `redirect_uri` value.
		 */
		redirectURI: string;
	}
	/**
	 * The request returned by {@link OAuth.PKCEClient.authorizationRequest}.
	 * Can be used as direct input to {@link OAuth.PKCEClient.authorize}, or
	 * to extract parameters for constructing a custom URL in {@link OAuth.AuthorizationOptions}.
	 */
	export interface AuthorizationRequest extends AuthorizationRequestURLParams {
		/**
		 * Constructs the full authorization URL.
		 */
		toURL(): string;
	}
	/**
	 * Options for customizing {@link OAuth.PKCEClient.authorize}.
	 * You can use values from {@link OAuth.AuthorizationRequest} to build your own URL.
	 */
	export interface AuthorizationOptions {
		/**
		 * The full authorization URL.
		 */
		url: string;
	}
	/**
	 * The response returned by {@link OAuth.PKCEClient.authorize}, containing the authorization code after the provider redirect.
	 * You can then exchange the authorization code for an access token using the provider's token endpoint.
	 */
	export interface AuthorizationResponse {
		/**
		 * The authorization code from the OAuth provider.
		 */
		authorizationCode: string;
	}
	/**
	 * Describes the TokenSet created from an OAuth provider's token response.
	 * The `accessToken` is the only required parameter but typically OAuth providers also return a refresh token, an expires value, and the scope.
	 * Securely store a token set via {@link OAuth.PKCEClient.setTokens} and retrieve it via {@link OAuth.PKCEClient.getTokens}.
	 */
	export interface TokenSet {
		/**
		 * The access token returned by an OAuth token request.
		 */
		accessToken: string;
		/**
		 * An optional refresh token returned by an OAuth token request.
		 */
		refreshToken?: string;
		/**
		 * An optional id token returned by an identity request (e.g. /me, Open ID Connect).
		 */
		idToken?: string;
		/**
		 * An optional expires value (in seconds) returned by an OAuth token request.
		 */
		expiresIn?: number;
		/**
		 * The optional space-delimited list of scopes returned by an OAuth token request.
		 * You can use this to compare the currently stored access scopes against new access scopes the extension might require in a future version,
		 * and then ask the user to re-authorize with new scopes.
		 */
		scope?: string;
		/**
		 * The date when the token set was stored via {@link OAuth.PKCEClient.setTokens}.
		 */
		updatedAt: Date;
		/**
		 * A convenience method for checking whether the access token has expired.
		 * The method factors in some seconds of "buffer", so it returns true a couple of seconds before the actual expiration time.
		 * This requires the `expiresIn` parameter to be set.
		 */
		isExpired(): boolean;
	}
	/**
	 * Options for a {@link OAuth.TokenSet} to store via {@link OAuth.PKCEClient.setTokens}.
	 */
	export interface TokenSetOptions {
		/**
		 * The access token returned by an OAuth token request.
		 */
		accessToken: string;
		/**
		 * An optional refresh token returned by an OAuth token request.
		 */
		refreshToken?: string;
		/**
		 * An optional id token returned by an identity request (e.g. /me, Open ID Connect).
		 */
		idToken?: string;
		/**
		 * An optional expires value (in seconds) returned by an OAuth token request.
		 */
		expiresIn?: number;
		/**
		 * The optional scope value returned by an OAuth token request.
		 */
		scope?: string;
	}
	/**
	 * Defines the standard JSON response for an OAuth token request.
	 * The response can be directly used to store a {@link OAuth.TokenSet} via {@link OAuth.PKCEClient.setTokens}.
	 */
	export interface TokenResponse {
		/**
		 * The `access_token` value returned by an OAuth token request.
		 */
		access_token: string;
		/**
		 * An optional `refresh_token` value returned by an OAuth token request.
		 */
		refresh_token?: string;
		/**
		 * An optional `id_token` value returned by an identity request (e.g. /me, Open ID Connect).
		 */
		id_token?: string;
		/**
		 * An optional `expires_in` value (in seconds) returned by an OAuth token request.
		 */
		expires_in?: number;
		/**
		 * The optional `scope` value returned by an OAuth token request.
		 */
		scope?: string;
	}
}

export class PKCEClient {
	redirectMethod: OAuth.RedirectMethod;
	providerName: string;
	providerIcon?: Image.ImageLike;
	providerId?: string;
	description?: string;
	private resolvesOnRedirect?: boolean;
	private isAuthorizing: boolean;

	constructor(options: OAuth.PKCEClient.Options) {
		this.providerId = options.providerId;
		this.providerName = options.providerName;
		this.providerIcon = options.providerIcon;
		this.description = options.description;
		this.redirectMethod = options.redirectMethod;
		this.resolvesOnRedirect = false;
		this.isAuthorizing = false;
	}

	buildAuthUrl(
		options: OAuth.AuthorizationRequestOptions,
		state: string,
		codeChallenge: string,
		redirectURI: string,
	) {
		const params = new URLSearchParams({
			response_type: "code",
			client_id: options.clientId,
			redirect_uri: redirectURI,
			scope: options.scope,
			state: state,
			code_challenge: codeChallenge,
			code_challenge_method: "S256",
			...options.extraParameters,
		});

		return `${options.endpoint}?${params}`;
	}

	getRedirectURI() {
		switch (this.redirectMethod) {
			case OAuth.RedirectMethod.Web:
				return "https://raycast.com/redirect?packageName=Extension";
			case OAuth.RedirectMethod.App:
				return "raycast://oauth?package_name=Extension";
			case OAuth.RedirectMethod.AppURI:
				return "com.raycast:/oauth?package_name=Extension";
		}
	}

	/**
	 * Creates an authorization request for the provided authorization endpoint, client ID, and scopes.
	 * You need to first create the authorization request before calling {@link OAuth.PKCEClient.authorize}.
	 *
	 * @remarks The generated code challenge for the PKCE request uses the S256 method.
	 *
	 * @returns A promise for an {@link OAuth.AuthorizationRequest} that you can use as input for {@link OAuth.PKCEClient.authorize}.
	 */
	async authorizationRequest(
		options: OAuth.AuthorizationRequestOptions,
	): Promise<OAuth.AuthorizationRequest> {
		const codeVerifier = randomBytes(32).toString("base64url");
		const codeChallenge = createHash("sha256")
			.update(codeVerifier)
			.digest("base64url");
		const state = Buffer.from(
			JSON.stringify({
				flavor: "release",
				id: randomUUID(),
				providerName: this.providerName,
			}),
		).toString("base64url");
		const redirectURI = this.getRedirectURI();

		return {
			state,
			codeChallenge,
			codeVerifier,
			redirectURI,
			toURL: () =>
				this.buildAuthUrl(options, state, codeChallenge, redirectURI),
		};
	}
	/**
	 * Starts the authorization and shows the OAuth overlay in Raycast.
	 * As parameter you can either directly use the returned request from {@link OAuth.PKCEClient.authorizationRequest},
	 * or customize the URL by extracting parameters from {@link OAuth.AuthorizationRequest} and providing your own URL via {@link AuthorizationOptions}.
	 * Eventually the URL will be used to open the authorization page of the provider in the web browser.
	 *
	 * @returns A promise for an {@link OAuth.AuthorizationResponse}, which contains the authorization code needed for the token exchange.
	 * The promise is resolved when the user was redirected back from the provider's authorization page to the Raycast extension.
	 */
	async authorize(
		options: OAuth.AuthorizationRequest | OAuth.AuthorizationOptions,
	): Promise<OAuth.AuthorizationResponse> {
		const isAuthorizationOptions = (
			s: OAuth.AuthorizationRequest | OAuth.AuthorizationOptions,
		): s is OAuth.AuthorizationOptions => {
			return typeof (s as any).url === "string";
		};

		const res = await bus.turboRequest("oauth.authorize", {
			client: {
				id: this.providerId,
				description: this.description ?? "Connect to your account",
				name: this.providerName,
				icon: this.providerIcon
					? serializeProtoImage(this.providerIcon)
					: undefined,
			},
			url: isAuthorizationOptions(options) ? options.url : options.toURL(),
		});

		if (!res.ok) {
			throw res.error;
		}

		return { authorizationCode: res.value.code };
	}

	private authorizationURL;
	/**
	 * Securely stores a {@link OAuth.TokenSet} for the provider. Use this after fetching the access token from the provider.
	 * If the provider returns a a standard OAuth JSON token response, you can directly pass the {@link OAuth.TokenResponse}.
	 * At a minimum, you need to set the {@link OAuth.TokenSet.accessToken}, and typically you also set {@link OAuth.TokenSet.refreshToken} and {@link OAuth.TokenSet.isExpired}.
	 * Raycast automatically shows a logout preference for the extension when a token set was saved.
	 *
	 * @remarks If you want to make use of the convenience {@link OAuth.TokenSet.isExpired} method, the property {@link OAuth.TokenSet.expiresIn} must be configured.
	 *
	 * @returns A promise that resolves when the token set has been stored.
	 */
	async setTokens(
		options: OAuth.TokenSetOptions | OAuth.TokenResponse,
	): Promise<void> {
		const isTokenResponse = (
			options: OAuth.TokenSetOptions | OAuth.TokenResponse,
		): options is OAuth.TokenResponse => {
			return Object.hasOwn(options, "access_token");
		};

		if (isTokenResponse(options)) {
			await bus.turboRequest("oauth.setTokens", {
				accessToken: options.access_token,
				refreshToken: options.refresh_token,
				idToken: options.id_token,
				scope: options.scope,
				expiresIn: options.expires_in,
				providerId: this.providerId,
			});
		} else {
			await bus.turboRequest("oauth.setTokens", {
				...options,
				providerId: this.providerId,
			});
		}
	}
	/**
	 * Retrieves the stored {@link OAuth.TokenSet} for the client.
	 * You can use this to initially check whether the authorization flow should be initiated or
	 * the user is already logged in and you might have to refresh the access token.
	 *
	 * @returns A promise that resolves when the token set has been retrieved.
	 */
	async getTokens(): Promise<OAuth.TokenSet | undefined> {
		const res = await bus.turboRequest("oauth.getTokens", {
			providerId: this.providerId,
		});
		const set = res.unwrap().tokenSet;

		if (!set) return undefined;

		const tokenSet = {
			accessToken: set.accessToken,
			refreshToken: set.refreshToken,
			scope: set.scope,
			idToken: set.idToken,
			updatedAt: new Date(set.updatedAt * 1000),
			expiresIn: set.expiresIn,
			isExpired: () => {
				return !!(
					tokenSet.expiresIn &&
					tokenSet.updatedAt.getTime() + tokenSet.expiresIn * 1000 < Date.now()
				);
			},
		};

		return tokenSet;
	}
	/**
	 * Removes the stored {@link OAuth.TokenSet} for the client.
	 */
	async removeTokens(): Promise<void> {
		await bus.turboRequest("oauth.removeTokens", {
			providerId: this.providerId,
		});
	}
}

export type TokenSet = {
	accessToken: string;
	refreshToken?: string;
	idToken?: string;
	expiresIn?: number;
	scope?: string;
	updatedAt: Date;
	isExpired: () => boolean;
};

export const OAuth = {
	PKCEClient,
	RedirectMethod: OauthRedirectMethod,
};
