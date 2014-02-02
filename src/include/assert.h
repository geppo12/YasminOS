/*
	 Copyright (c) 2013 Giuseppe Monteleone, All rights reserved.
	 contact info@ing-monteleone.com

	 This file is part of 'YasminOS'

	 GNU Lesser General Public License Usage
	 this file may be used under the terms of the GNU Lesser
	 General Public License version 2.1 as published by the Free Software
	 Foundation and appearing in the file LICENSE.LGPL included in the
	 packaging of this file.  Please review the following information to
	 ensure the GNU Lesser General Public License version 2.1 requirements
	 will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.

	 THIS SOFTWARE IS PROVIDED BY GIUSEPPE MONTELEONE ``AS IS'' AND ANY
	 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
	 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL GIUSEPPE MONTELEONE BE
	 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
	 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
	 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
	 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
	 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
	 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	 You should have received a copy of the GNU General Public License
	 along with 'YasminOS'. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ASSERT_H_
#define ASSERT_H_

#if defined(USE_ASSERT) && !defined(NDEBUG)
/* trick: double macro expansion convert __LINE__ to string at compile time so we don't need code
   for itoa when we don't use YOS_DbgPrintf
   detail:
   first expansion (ASSERT_STR) convert __LINE__ to an integer
   second expansion (ASSERT_STR2) convert integer to string */
#define ASSERT_STR2(a) #a
#define ASSERT_STR(a) ASSERT_STR2(a)
// no comma: after expansion are all constant sting so are merged in a single string
#define ASSERT(x)   do { if (!(x)) YOS_AssertFail("ASSERT FAIL: " ASSERT_STR(x) " " __FILE__ " " ASSERT_STR(__LINE__) "\n"); } while(0)
#else
#define ASSERT(x)	do {} while(0)
#endif

typedef void (*YOS_AssertSignal_t)(void);
// user can set this pointer to function to signal assert failure condition
// we use global var because take less code
extern YOS_AssertSignal_t gYosAssertSignal;

void YOS_AssertFail(char *conditionStr);

#endif /* ASSERT_H_ */
