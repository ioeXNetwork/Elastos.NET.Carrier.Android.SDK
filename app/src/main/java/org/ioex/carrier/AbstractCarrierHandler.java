/*
 * Copyright (c) 2018 Elastos Foundation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * Copyright (c) 2019 ioeXNetwork
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

package org.ioex.carrier;

import java.util.List;

/**
 * The abstract carrier node handler class.
 */

public abstract class AbstractCarrierHandler implements CarrierHandler {
	/**
	 * The callback function that perform idle work.
	 *
	 * @param
	 * 		carrier		Carrier node instance
	 */
	public void onIdle(Carrier carrier) {}

	/**
	 * The callback function to process the self connection status.
	 *
	 * @param
	 * 		carrier		Carrier node instance
	 * @param
	 * 		status 		Current connection status. @see ConnectionStatus
	 */
	public void onConnection(Carrier carrier, ConnectionStatus status) {}

	/**
	 * The callback function to process the ready notification.
	 *
	 * Application should wait this callback invoked before calling any carrier
	 * function to interact with friends.
	 *
	 * @param
	 * 		carrier 	Carrier node instance
	 */
	public void onReady(Carrier carrier) {}

	/**
	 * The callback function to process the self info changed event.
	 *
	 * @param
	 * 		carrier 	Carrier node instance
	 * @param
	 * 		info 	The updated user information
	 */
	public void onSelfInfoChanged(Carrier carrier, UserInfo info) {}

	/**
	 * The callback function to iterate the each friend item in friend list.
	 *
	 * @param
	 * 		carrier    	Carrier node instance
	 * @param
	 * 		friends 	The friends list.
	 */
	public void onFriends(Carrier carrier, List<FriendInfo> friends) {}

	/**
	 * The callback function to process the friend connections status changed event.
	 *
	 * @param
	 * 		carrier    	carrier node instance.
	 * @param
	 * 		friendId 	The friend's user id.
	 * @param
	 * 		status	    The connection status of friend. @see ConnectionStatus
	 */
	public void onFriendConnection(Carrier carrier, String friendId, ConnectionStatus status) {}

	/**
	 * The callback function to process the friend information changed event.
	 *
	 * @param
	 * 		carrier		Carrier node instance
	 * @param
	 * 		friendId   	The friend's user id
	 * @param
	 * 		info		The update friend information
	 */
	public void onFriendInfoChanged(Carrier carrier, String friendId, FriendInfo info) {}

	/**
	 * The callback function to process the friend presence changed event.
	 *
	 * @param
	 * 		carrier    	Carrier node instance
	 * @param
	 * 		friendId   	The friend's user id
	 * @param
	 * 		presence	The presence status of the friend
	 */
	public void onFriendPresence(Carrier carrier, String friendId, PresenceStatus presence) {}

	/**
	 * The callback function to process the friend request.
	 *
	 * @param
	 * 		carrier    	Carrier node instance
	 * @param
	 * 		userId    	The user id who want be friend with current user
	 * @param
	 * 		info		The user information to `userId`
	 * @param
	 * 		hello      	The PIN for target user, or any application defined content
	 */
	public void onFriendRequest(Carrier carrier, String userId, UserInfo info, String hello) {}

	/**
	 * The callback function to process the new friend added event.
	 *
	 * @param
	 * 		carrier		Carrier node instance
	 * @param
	 * 		info	The added friend's information
	 */
	public void onFriendAdded(Carrier carrier, FriendInfo info) {}

	/**
	 * The callback function to process the friend removed event.
	 *
	 * @param
	 * 		carrier		Carrier node instance
	 * @param
	 * 		friendId   	The friend's user id
	 */
	public void onFriendRemoved(Carrier carrier, String friendId) {}

	/**
	 * The callback function to process the friend message.
	 *
	 * @param
	 * 		carrier   	Carrier node instance
	 * @param
	 * 		from     	The id(userid@nodeid) from who send the message
	 * @param
	 * 		message   	The message content
	 */
	public void onFriendMessage(Carrier carrier, String from, String message) {}

	/**
	 * The callback function to process the friend invite request.
	 *
	 * @param
	 * 		carrier   	Carrier node instance
	 * @param
	 * 		from       	The user id from who send the invite request
	 * @param
	 * 		data       	The application defined data sent from friend
	 */
	public void onFriendInviteRequest(Carrier carrier, String from, String data) {}

	/**
	 * The callback function to process the friend file request.
	 *
	 * @param
	 * 		carrier   	Carrier node instance
	 * @param
	 * 		from       	The user id from who send the file request
	 * @param
	 * 		fileid       	The file id of the sending file
	 * @param
	 * 		filename       	The file name sent from friend
	 * @param
	 * 		filesize       	The file size sent from friend
	 */
	public void onFriendFileRequest(Carrier carrier, String from, String fileid, String filename, long filesize) {}

	/**
	 * The callback function to process the friend file accepted.
	 *
	 * @param
	 * 		carrier   	Carrier node instance
	 * @param
	 * 		receiver     The user id who accepts the file sending request
	 * @param
	 * 		fileid       	The file id accepted by receiver
	 * @param
	 * 		filepath      The full file path accepted by receiver
	 * @param
	 * 		filesize       The file size accepts by receiver
	 */
	public void onFriendFileAccepted(Carrier carrier, String receiver, String fileid, String filepath, long filesize) {}

	/**
	 * The callback function to notify the file is paused.
	 *
	 * @param
	 * 		carrier   	Carrier node instance
	 * @param
	 * 		friendid    The friend id
	 * @param
	 * 		fileid       	 The file id
	 */
	public void onFriendFilePaused(Carrier carrier, String friendid, String fileid) {}

	/**
	 * The callback function to notify the file is resumed.
	 *
	 * @param
	 * 		carrier   	Carrier node instance
	 * @param
	 * 		friendid    The friend id
	 * @param
	 * 		fileid       	The fileid
	 */
	public void onFriendFileResumed(Carrier carrier, String friendid, String fileid) {}

	/**
	 * The callback function to notify the file is canceled.
	 *
	 * @param
	 * 		carrier   	Carrier node instance
	 * @param
	 * 		friendid    The friend id
	 * @param
	 * 		fileid       	The fileid
	 */
	public void onFriendFileCanceled(Carrier carrier, String friendid, String fileid) {}

	/**
	 * The callback function to notify the file is completed.
	 *
	 * @param
	 * 		carrier   	Carrier node instance
	 * @param
	 * 		friendid    The friend id
	 * @param
	 * 		fileid       	The fileid
	 */
	public void onFriendFileCompleted(Carrier carrier, String friendid, String fileid) {}

	/**
	 * The callback function to process the friend file transferred.
	 *
	 * @param
	 * 		carrier   	Carrier node instance
	 * @param
	 * 		friendid    The user id
	 * @param
	 * 		filepath     The full path of the transferred file
	 * @param
	 * 		fileid       	The file id
	 * @param
	 * 		totalsize    The total file size
	 * @param
	 * 		transferredsize      The transferred file size
	 */
	public void onFriendFileProgress(Carrier carrier, String friendid, String filepath, String fileid, long totalsize, long transferredsize) {}

	/**
	 * The callback function to process the friend file queried.
	 *
	 * @param
	 * 		carrier   	Carrier node instance
	 * @param
	 * 		userid     The user id who queries the file
	 * @param
	 * 		filename     The queried file name
	 * @param
	 * 		message      The extra message from friend
	 */
	public void onFriendFileQueried(Carrier carrier, String userid, String filename, String message) {}
}