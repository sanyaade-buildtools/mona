/*
 *
 *   Copyright (c) 2011  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef _FACEBOOK_POST_
#define _FACEBOOK_POST_

struct Comment
{
    Comment(const std::string& id, const std::string& body) : id(id), body(body)
    {
    }
    std::string id;
    std::string body;
 };

typedef std::vector<Comment> Comments;

struct FacebookPost
{
    FacebookPost(const std::string& imageId,
                 const std::string& name,
                 const std::string& text,
                 uint32_t numLikes,
                 const std::string& postId,
                 uint32_t numComments,
                 const Comments& comments
        ) :
        imageId(imageId),
        name(name),
        text(text),
        numLikes(numLikes),
        postId(postId),
        numComments(numComments),
        comments(comments)
    {
    }

    std::string imageUrl() const
    {
        std::string ret = "http://graph.facebook.com/";
        ret += imageId;
        ret += "/picture";
        return ret;
    }

    std::string localImagePath() const
    {
        std::string ret = "/USER/TEMP/" + imageId + ".JPG";
        return ret;
    }

    std::string imageId;
    std::string name;
    std::string text;
    uint32_t numLikes;
    std::string postId;
    uint32_t numComments;
    Comments comments;
};

#endif // _FACEBOOK_POST_
