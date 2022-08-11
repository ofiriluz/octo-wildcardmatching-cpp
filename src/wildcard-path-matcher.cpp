/**
 * @file wildcard-path-matcher.cpp
 * @author ofir iluz (iluzofir@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-08-11
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "octo-wildcardmatching-cpp/wildcard-path-matcher.hpp"
#include <string.h>

namespace
{
static constexpr char UNIX_FOLDER_SEPERATOR_CHAR = '/';
static constexpr char WINDOWS_FOLDER_SEPERATOR_CHAR = '\\';
static constexpr char SINGLE_WILDCARD_CHAR = '*';
static constexpr char SINGLE_WILDCARD_STRING[] = "*";
static constexpr char DOUBLE_WILDCARD_STRING[] = "**";
} // namespace

namespace octo::wildcardmatching
{

WildcardPathMatcher::Wildcard::Wildcard(size_t begin, size_t end)
{
    offset = begin;
    size = end - begin;
}

WildcardPathMatcher::WildcardPathMatcher(bool allow_last_wildcard_as_many_paths)
{
    allow_last_wildcard_as_many_paths_ = allow_last_wildcard_as_many_paths;
    evaluate_os_folder_seperator();
}

WildcardPathMatcher::~WildcardPathMatcher()
{
}

void WildcardPathMatcher::evaluate_os_folder_seperator()
{
#ifdef WIN_ENV
    folder_seperator_ = WINDOWS_FOLDER_SEPERATOR_CHAR;
#else
    folder_seperator_ = UNIX_FOLDER_SEPERATOR_CHAR;
#endif
}

std::vector<std::string> WildcardPathMatcher::split_string_by_delimiter(const std::string& input,
                                                                          char delimiter) const
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream token_stream(input);
    while (std::getline(token_stream, token, delimiter))
    {
        // If this is an empty string, move on, case of lots of /// in the same path part
        if (token == "")
        {
            continue;
        }

        tokens.push_back(token);
    }

    return tokens;
}

std::vector<WildcardPathMatcher::Wildcard> WildcardPathMatcher::split_by_wildcards(
    const std::string& wildcard_str) const
{
    // Split the wildcard string by * and keep offset and size
    size_t pos = wildcard_str.find('*');
    std::vector<Wildcard> wildcards;
    if (pos == std::string::npos)
    {
        // If no * is found, its a normal string and this means we only have one wildcard part
        wildcards.push_back(Wildcard(0, wildcard_str.size()));
    }
    else
    {
        // Add the prefix wildcard part
        wildcards.push_back(Wildcard(0, pos));
        ++pos;
        // Start collecting all the infix wildcard parts
        for (;;)
        {
            size_t pos2 = wildcard_str.find('*', pos);
            if (pos2 == std::string::npos)
            {
                break;
            }
            if (pos2 != pos)
            {
                wildcards.push_back(Wildcard(pos, pos2));
            }
            pos = pos2 + 1;
        }
        // Add the postfix wildcard part
        wildcards.push_back(Wildcard(pos, wildcard_str.size()));
    }

    return wildcards;
}

bool WildcardPathMatcher::compare_validated_wildcard_strings(const std::string& wildcard_str,
                                                               const std::string& input_str) const
{
    // Need to look for each wildcard segment only once within the string part
    // Start by creating the wildcard parts and keep their size and index
    std::vector<Wildcard> wildcards = split_by_wildcards(wildcard_str);

    // Start iterating over the string
    const char* begin = input_str.c_str();
    const char* end = begin + input_str.size();

    // Check prefix card
    const Wildcard& prefix_card = wildcards.front();
    // Assert size
    if (size_t(end - begin) < prefix_card.size)
    {
        return false;
    }
    std::string::const_iterator card_begin = wildcard_str.begin() + prefix_card.offset;
    // Assert that the prefix card is equal
    if (!std::equal(begin, begin + prefix_card.size, card_begin))
    {
        return false;
    }
    // Move the pointer
    begin += prefix_card.size;

    // Check if we only have one wildcard (probably means no *)
    if (wildcards.size() == 1)
    {
        return begin == end;
    }

    // Check suffix card
    const Wildcard& suffix_card = wildcards.back();
    // Assert size
    if (size_t(end - begin) < suffix_card.size)
    {
        return false;
    }
    card_begin = wildcard_str.begin() + suffix_card.offset;
    // Assert that the prefix card is equal
    if (!std::equal(end - suffix_card.size, end, card_begin))
    {
        return false;
    }
    // Move the pointer
    end -= suffix_card.size;

    // Check infix cards
    for (size_t i = 1; i != wildcards.size() - 1; ++i)
    {
        const Wildcard& infix_card = wildcards[i];
        std::string::const_iterator card_begin = wildcard_str.begin() + infix_card.offset;
        std::string::const_iterator card_end = card_begin + infix_card.size;
        // Assert that we can find one within the card infix
        begin = std::search(begin, end, card_begin, card_end);
        if (begin == end)
        {
            return false;
        }
        // Move the pointer
        begin += infix_card.size;
    }

    return true;
}

bool WildcardPathMatcher::compare_validated_wildcard_paths(const std::vector<std::string>& input_path_parts,
                                                             const std::string& wildcard_path) const
{
    // Split the wildcard path
    std::vector<std::string> wildcard_path_parts = split_string_by_delimiter(wildcard_path, folder_seperator_);

    // Start going over the wildcard path parts and validate them against the input path
    int wildcard_path_part_index = 0;
    int input_path_part_index = 0;

    while (input_path_part_index < (int)input_path_parts.size() &&
           wildcard_path_part_index < (int)wildcard_path_parts.size())
    {
        // This only happens either we have a double wildcard for many paths
        // Or we have a single wildcard on the last path part as a standalone and it is allowed
        if (strncmp(wildcard_path_parts[wildcard_path_part_index].c_str(),
                    DOUBLE_WILDCARD_STRING,
                    strlen(DOUBLE_WILDCARD_STRING)) == 0 ||
            should_allow_last_wildcard_as_many_paths(
                wildcard_path_parts[wildcard_path_part_index], wildcard_path_parts, wildcard_path_part_index))
        {
            // Check double wildcard comparison
            // If return true, this means we need to finish the loop
            if (handle_double_wildcard_part_comparison(
                    wildcard_path_parts, input_path_parts, wildcard_path_part_index, input_path_part_index))
            {
                break;
            }
        }
        else
        {
            // Check normal wildcard comparison
            // If return true, this means we need to finish the loop
            if (handle_normal_part_comparison(
                    wildcard_path_parts, input_path_parts, wildcard_path_part_index, input_path_part_index))
            {
                break;
            }
        }
    }

    // Make the final decision whether we found a match or not
    return decide_if_match_is_found(
        wildcard_path_parts, input_path_parts, wildcard_path_part_index, input_path_part_index);
}

bool WildcardPathMatcher::handle_double_wildcard_part_comparison(const std::vector<std::string>& wildcard_path_parts,
                                                                   const std::vector<std::string>& input_path_parts,
                                                                   int& wildcard_path_part_index,
                                                                   int& input_path_part_index) const
{
    // Loop until we find the last ** in a row, just to avoid double searching
    while (wildcard_path_part_index < (int)wildcard_path_parts.size() &&
           strncmp(wildcard_path_parts[wildcard_path_part_index].c_str(),
                   DOUBLE_WILDCARD_STRING,
                   strlen(DOUBLE_WILDCARD_STRING)) == 0)
    {
        wildcard_path_part_index++;
    }

    // If this is the last part, this means we found a match since it ends with only **
    if (wildcard_path_part_index == (int)wildcard_path_parts.size())
    {
        // Move the index to the end since it fits everything and finish here
        input_path_part_index = input_path_parts.size();
        return true;
    }

    // Start collecting all the wildcard parts until the next **
    // This will be the infix parts we will check
    std::vector<std::string> wildcard_infix_parts;
    while (wildcard_path_part_index < (int)wildcard_path_parts.size() &&
           strncmp(wildcard_path_parts[wildcard_path_part_index].c_str(),
                   DOUBLE_WILDCARD_STRING,
                   strlen(DOUBLE_WILDCARD_STRING)) != 0)
    {
        wildcard_infix_parts.push_back(wildcard_path_parts[wildcard_path_part_index]);
        wildcard_path_part_index++;
    }

    // Check if we can find all those parts within the input somewhere
    // If we found it somewhere, that means that the infix is good for us
    // We can move to the found index on the input and continue from there
    int current_infix_index = 0;
    int current_path_part_index = input_path_part_index;
    while (current_path_part_index != (int)input_path_parts.size() &&
           current_infix_index != (int)wildcard_infix_parts.size())
    {
        if (compare_validated_wildcard_strings(wildcard_infix_parts[current_infix_index],
                                               input_path_parts[current_path_part_index]))
        {
            current_infix_index++;
        }
        else
        {
            current_path_part_index -= current_infix_index;
            current_infix_index = 0;
        }
        current_path_part_index++;
    }
    // We found the infix
    if (current_infix_index == (int)wildcard_infix_parts.size())
    {
        input_path_part_index = current_path_part_index;
    }
    // We did not find the infix, we can stop here
    // We also move the wildcard index back since we did not find
    else
    {
        wildcard_path_part_index -= wildcard_infix_parts.size();
        return true;
    }
    // If this is the last part, this means we might have found a match
    // Check the last parts to validate it
    // Either we have ** or * and allowed in the end
    if (wildcard_path_part_index == (int)wildcard_path_parts.size() &&
        (strncmp(wildcard_path_parts[wildcard_path_part_index - 1].c_str(),
                 DOUBLE_WILDCARD_STRING,
                 strlen(DOUBLE_WILDCARD_STRING)) == 0 ||
         should_allow_last_wildcard_as_many_paths(
             wildcard_path_parts[wildcard_path_part_index - 1], wildcard_path_parts, wildcard_path_part_index - 1)))
    {
        // Move the index to the end since it fits everything and finish here
        input_path_part_index = input_path_parts.size();
        return true;
    }
    // Or the last part can be compared to all the suffix exactly
    // In the infix part, we only search somewhere in the parts for a match
    // Here we must assert that the last part, the suffix is equal completely
    else if (wildcard_path_part_index == (int)wildcard_path_parts.size())
    {
        // Collect last suffix
        std::vector<std::string> wildcard_postfix_parts;
        int wildcard_path_postfix_index = wildcard_path_parts.size() - 1;
        while (wildcard_path_postfix_index >= 0 && strncmp(wildcard_path_parts[wildcard_path_postfix_index].c_str(),
                                                           DOUBLE_WILDCARD_STRING,
                                                           strlen(DOUBLE_WILDCARD_STRING)) != 0)
        {
            wildcard_postfix_parts.insert(wildcard_postfix_parts.begin(),
                                          wildcard_path_parts[wildcard_path_postfix_index]);
            wildcard_path_postfix_index--;
        }
        wildcard_path_postfix_index = 0;
        int input_path_postfix_index = input_path_parts.size() - wildcard_postfix_parts.size();
        // Check if the last part fits the remainder of the input
        while (input_path_postfix_index < (int)input_path_parts.size() &&
               wildcard_path_postfix_index < (int)wildcard_postfix_parts.size())
        {
            if (compare_validated_wildcard_strings(wildcard_postfix_parts[wildcard_path_postfix_index],
                                                   input_path_parts[input_path_postfix_index]))
            {
                input_path_postfix_index++;
                wildcard_path_postfix_index++;
            }
            else
            {
                break;
            }
        }
        // If we reached the end on both we are good
        if (input_path_postfix_index == (int)input_path_parts.size() &&
            wildcard_path_postfix_index == (int)wildcard_postfix_parts.size())
        {
            input_path_part_index = input_path_parts.size();
        }
    }

    return false;
}

bool WildcardPathMatcher::handle_normal_part_comparison(const std::vector<std::string>& wildcard_path_parts,
                                                          const std::vector<std::string>& input_path_parts,
                                                          int& wildcard_path_part_index,
                                                          int& input_path_part_index) const
{
    if (!compare_validated_wildcard_strings(wildcard_path_parts[wildcard_path_part_index],
                                            input_path_parts[input_path_part_index]))
    {
        // We did not find a match, this means we can stop here and continue to the next wildcard path
        return true;
    }
    else
    {
        // Promote both the indices since we found a match within the single wildcard
        input_path_part_index++;
        wildcard_path_part_index++;
    }

    return false;
}

bool WildcardPathMatcher::decide_if_match_is_found(const std::vector<std::string>& wildcard_path_parts,
                                                     const std::vector<std::string>& input_path_parts,
                                                     int& wildcard_path_part_index,
                                                     int& input_path_part_index) const
{
    // Make sure to handle finishing ** if we reached the end of the input
    while (wildcard_path_part_index < (int)wildcard_path_parts.size() &&
           strncmp(wildcard_path_parts[wildcard_path_part_index].c_str(),
                   DOUBLE_WILDCARD_STRING,
                   strlen(DOUBLE_WILDCARD_STRING)) == 0)
    {
        wildcard_path_part_index++;
    }

    // If we reached the end of the input and wildcard, this means we properly found a match
    if ((wildcard_path_part_index == (int)wildcard_path_parts.size() &&
         input_path_part_index == (int)input_path_parts.size()) ||
        (input_path_part_index == (int)input_path_parts.size() &&
         wildcard_path_part_index == (int)wildcard_path_parts.size() - 1 &&
         strncmp(wildcard_path_parts[wildcard_path_part_index].c_str(),
                 SINGLE_WILDCARD_STRING,
                 strlen(SINGLE_WILDCARD_STRING)) == 0 &&
         allow_last_wildcard_as_many_paths_))
    {
        return true;
    }
    return false;
}

bool WildcardPathMatcher::should_allow_last_wildcard_as_many_paths(
    const std::string& wildcard_path_part,
    const std::vector<std::string>& wildcard_path_parts,
    size_t wildcard_path_part_index) const
{
    return allow_last_wildcard_as_many_paths_ &&
           (wildcard_path_part[wildcard_path_part.size() - 1] == SINGLE_WILDCARD_CHAR) &&
           (wildcard_path_part_index + 1) == wildcard_path_parts.size();
}

bool WildcardPathMatcher::get_allow_last_wildcard_as_many_paths() const
{
    return allow_last_wildcard_as_many_paths_;
}

void WildcardPathMatcher::set_allow_last_wildcard_as_many_paths(bool allow_last_wildcard_as_many_paths)
{
    allow_last_wildcard_as_many_paths_ = allow_last_wildcard_as_many_paths;
}

char WildcardPathMatcher::get_folder_seperator() const
{
    return folder_seperator_;
}

void WildcardPathMatcher::set_folder_seperator(char folder_seperator)
{
    folder_seperator_ = folder_seperator;
}

bool WildcardPathMatcher::validate_wildcard_path(const std::string& wildcard_path) const
{
    std::vector<std::string> path_parts = split_string_by_delimiter(wildcard_path, folder_seperator_);
    // Check the parts of the wildcard path to see if they are logical or not
    for (std::vector<std::string>::iterator part_iter = path_parts.begin(); part_iter != path_parts.end(); ++part_iter)
    {
        for (size_t i = 0; i < part_iter->size(); i++)
        {
            char c = (*part_iter)[i];
            if (c == SINGLE_WILDCARD_CHAR)
            {
                if ((i + 1) < part_iter->size() && (*part_iter)[i + 1] == SINGLE_WILDCARD_CHAR)
                {
                    // Found **, check that they are the only one
                    if (part_iter->size() == strlen(DOUBLE_WILDCARD_STRING))
                    {
                        // We good
                        break;
                    }
                    else
                    {
                        // We bad
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

void WildcardPathMatcher::add_wildcard_path(const std::string& wildcard_path)
{
    if (!validate_wildcard_path(wildcard_path))
    {
        throw std::runtime_error(std::string("The path is invalid: [") + wildcard_path + "]");
    }

    wildcard_paths_.push_back(wildcard_path);
}

void WildcardPathMatcher::add_wildcard_paths(const std::vector<std::string>& wildcard_paths)
{
    // Validate all paths before adding them
    for (std::vector<std::string>::const_iterator iter = wildcard_paths.begin(); iter != wildcard_paths.end(); ++iter)
    {
        if (!validate_wildcard_path(*iter))
        {
            throw std::runtime_error(std::string("A path is invalid: [") + *iter + "]");
        }
    }

    // Add them only if they were all valid in the previous loop
    for (std::vector<std::string>::const_iterator iter = wildcard_paths.begin(); iter != wildcard_paths.end(); ++iter)
    {
        wildcard_paths_.push_back(*iter);
    }
}

void WildcardPathMatcher::clean_wildcard_paths()
{
    wildcard_paths_.clear();
}

std::vector<std::string> WildcardPathMatcher::get_wildcard_paths() const
{
    return wildcard_paths_;
}

bool WildcardPathMatcher::has_match(const std::string& input) const
{
    return get_wildcard_match(input) != "";
}

std::string WildcardPathMatcher::get_wildcard_match(const std::string& input) const
{
    // Split the input path to the base paths list
    std::vector<std::string> input_path_parts = split_string_by_delimiter(input, folder_seperator_);

    // Go over every writable path and see if we can find a fit
    for (std::vector<std::string>::const_iterator wildcard_path_iter = wildcard_paths_.begin();
         wildcard_path_iter != wildcard_paths_.end();
         ++wildcard_path_iter)
    {
        // Compare the parts with the current wildcard path
        if (compare_validated_wildcard_paths(input_path_parts, *wildcard_path_iter))
        {
            return *wildcard_path_iter;
        }
    }

    return "";
}
} // namespace octo::wildcardmatching